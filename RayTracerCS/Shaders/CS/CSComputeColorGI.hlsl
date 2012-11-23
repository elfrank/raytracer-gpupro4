// ================================================================================ //
// The authors of this code (Francisco Ávila, Sergio Murguía, Leo Reyes and Arturo	//
// García) hereby release it into the public domain. This applies worldwide.		//
//																					//
// In case this is not legally possible, we grant any entity the right to use this	//
// work for any purpose, without any conditions, unless such conditions are			//
// required by law.																	//
//																					//
// This work is provided on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF	//
// ANY KIND. You are solely responsible for the consequences of using or			//
// redistributing this code and assume any risks associated with these or related	//
// activities.																		//
// ================================================================================ //

//--------------------------------------------------------------------------------------------------------------------
// COLOR STAGE
//--------------------------------------------------------------------------------------------------------------------
// Apply shadows and iMaterial color.
[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]
void CSComputeColor(uint3 DTid : SV_DispatchThreadID, uint GIndex : SV_GroupIndex)
{
	// Calculate 1D index of the current thread
	const unsigned int index = DTid.y * N + DTid.x;

	// Get the triangle ID stored in the current pixel
	const int iTriangleId = g_uIntersections[index].iTriangleId;	

	// If Environment Mapping has already been applied to this ray, return.
	
	if(iTriangleId < -2) 
	{
		return;
	}
	

	float2 indexasfloat = float2(DTid.x,DTid.y)/1024.0f;
	g_uIntersections[index] = g_uIntersections[index];

	g_uRays[index] = g_uRays[index];
	float3 fFactor = g_uRays[index].vfReflectiveFactor;

	// Init variables
	const int currentMaterial = g_sPrimitivesData[iTriangleId].iMaterialId;
	
	// Ray hit something, apply iMaterial, otherwise apply env mapping and kill ray
	if (iTriangleId>=0)
	{
		const int currentMaterial = g_sPrimitivesData[iTriangleId].iMaterialId;

		float3 vfNormal = 1.f;
		float3 vfNormalNM = 1.f;

		float2 vfUV = computeUVs(iTriangleId, g_uIntersections[index]);
		// Shading (Phong Shading or Flat Shading)
		// Generate normal
		const unsigned int offset = iTriangleId * 3;
		Vertex A = g_sVertices[g_sIndices[offset]];
		Vertex B = g_sVertices[g_sIndices[offset+1]];
		Vertex C = g_sVertices[g_sIndices[offset+2]];
		float3 Edge1 = B.vfPosition - A.vfPosition;
		float3 Edge2 = C.vfPosition - A.vfPosition;
		float3 NorT = normalize(cross(Edge1, Edge2));
		bool inverseNormal=false;

		if(g_bIsPhongShadingOn)
		{
			// vfNormal using phong interpolation (per pixel)
			vfNormal = normalize(Vec3BaryCentric(
				A.vfNormal, B.vfNormal, C.vfNormal, 
				float2(g_uIntersections[index].fU,g_uIntersections[index].fV)));
		}
		else
		{
			// vfNormal using flat interpolation (per triangle)
			vfNormal = NorT;
		}

		//vfNormal = 1.f;

		// Calculate a normal vector using vfNormal Mapping
		if(g_bIsNormalMapspingOn)
		{
			// Read values from the normal-map texture 
			// normal, tangent and bitangent
			float3 tx_g_sNormalMaps = g_sNormalMaps.SampleLevel(
				g_ssSampler, float3(vfUV,currentMaterial),0).xyz;
			if(length(tx_g_sNormalMaps) < 0.1)
			{
				tx_g_sNormalMaps = float3(0.5f, 0.5f, 1.0f);
			}

			// Center the texture
			tx_g_sNormalMaps = 2 * tx_g_sNormalMaps-1;

			float2 Edge1UV = g_sVertices[g_sIndices[offset+1]].vfUvs - g_sVertices[g_sIndices[offset]].vfUvs;
			float2 Edge2UV = g_sVertices[g_sIndices[offset+2]].vfUvs - g_sVertices[g_sIndices[offset]].vfUvs;
			const float cp = Edge1UV.y*Edge2UV.x - Edge1UV.x*Edge2UV.y;

			if(cp != 0.f)
			{
				float mult = 1.f/cp;
				float3 tangent, bitangent;
				tangent = (Edge2*Edge1UV.y - Edge1*Edge2UV.y)*mult;
				bitangent = (Edge2*Edge1UV.x - Edge1*Edge2UV.x)*mult;
				tangent -= vfNormal*dot(tangent, vfNormal);
				tangent = normalize(tangent);
				bitangent -= vfNormal*dot(bitangent, vfNormal);
				bitangent -= tangent*dot(bitangent, tangent);
				bitangent = normalize(bitangent);
				vfNormalNM = tx_g_sNormalMaps.z*vfNormal +
							tx_g_sNormalMaps.x*tangent -
							tx_g_sNormalMaps.y*bitangent; 
			}
			else
			{
				vfNormalNM = vfNormal;
			}
		}
		else
		{
			vfNormalNM = vfNormal;
		}
		// END IF vfNormal Mapping
				
		//point normals to face ray source
		if (dot(NorT,g_uRays[index].vfDirection) > 0.f)
		{
			NorT = -NorT;
			inverseNormal=true;
		}
		if (dot(vfNormalNM,g_uRays[index].vfDirection) > 0.f)
		{
			vfNormalNM = -vfNormalNM;
		}
		
		// Generate hit point and light dir
		float3 vfHitPoint = g_uRays[index].vfOrigin + g_uIntersections[index].fT * g_uRays[index].vfDirection;

		//apply Lambert iMaterial		
		float4 fAmbientLight = g_sMaterials[currentMaterial].fAmbient;
		float4 tx_TextureColor = g_sMaterials[currentMaterial].fDiffuse;
		float4 tx_SpecularColor = g_sMaterials[currentMaterial].fSpecular;

		if( g_bIsTexturingOn )
		{
			fAmbientLight.xyz *= g_sTextures.SampleLevel(g_ssSampler, float3(vfUV,currentMaterial),0).xyz;
			tx_TextureColor.xyz *= g_sTextures.SampleLevel(g_ssSampler, float3(vfUV,currentMaterial),0).xyz;
			tx_SpecularColor.xyz *= g_sSpecularMaps.SampleLevel(g_ssSampler, float3(vfUV,currentMaterial),0).xyz;
		}

		tx_SpecularColor.w *= g_bIsGlossMappingOn;

		// Ray bounce
		g_uRays[index].vfOrigin = vfHitPoint;
		float flatness = tx_SpecularColor.w;
		float Pdiff = 1.0-g_sMaterials[currentMaterial].fShininess;
		float Preflection = g_sMaterials[currentMaterial].fAlpha;
		float refindex = fAmbientLight.a;
		//float flatness = 1.0;//tx_SpecularColor.x;
		//float Pdiff = -10.0;//1.f - tx_SpecularColor.x;
		//float Preflection = 1.f;
		float4 randomDir = g_sRandomTx.SampleLevel(g_ssSampler, indexasfloat+float2(g_fRandX,g_fRandY), 0);
		if (Preflection >= randomDir.w)
		{
			if ((Pdiff*Preflection)>=randomDir.w)
			{
				//diffuse
				g_uRays[index].vfReflectiveFactor *= tx_TextureColor.xyz;
				g_uRays[index].vfDirection = normalize(randomDir.xyz);
			}
			else
			{
				//reflection
				g_uRays[index].vfReflectiveFactor *= tx_SpecularColor.xyz;
				g_uRays[index].vfDirection = normalize((1.f-flatness)*randomDir.xyz+flatness*reflect(g_uRays[index].vfDirection,vfNormalNM));
			}
			if (dot(g_uRays[index].vfDirection,vfNormalNM)<0)
			{
				g_uRays[index].vfDirection = -g_uRays[index].vfDirection;
			}
			if (dot(g_uRays[index].vfDirection,NorT)<0)
			{
				g_uRays[index].vfDirection = reflect(g_uRays[index].vfDirection,NorT);
			}			
		}
		else
		{
			//refraction
			float refindex = fAmbientLight.a;
			if (!inverseNormal)
			{
				refindex=1.0/refindex;			
			}
			//falta usar coeficientes de fressnell y beers law
			float cosi = -dot(g_uRays[index].vfDirection,vfNormalNM);
			float cost = 1-((1-(cosi*cosi))*refindex*refindex);
			if (cost<0){
			    //total inner reflection
			    g_uRays[index].vfDirection = reflect(g_uRays[index].vfDirection , vfNormalNM);
			} else {
				float Rs = (cosi-refindex*cost)/(cosi+refindex*cost);
				Rs*=Rs;
				float Rp = (cost-refindex*cosi)/(cost+refindex*cosi);
				Rp*=Rp;
				float R=Rs;
				if (R*(1.0-Preflection)>=randomDir.w) {
					//reflection from Fresnel
					g_uRays[index].vfDirection = reflect(g_uRays[index].vfDirection , vfNormalNM);
				} else {				
					float3 refractdir = refract(g_uRays[index].vfDirection , vfNormalNM, refindex );
					float refnorm = length(refractdir);
					g_uRays[index].vfDirection = refractdir/refnorm;
					g_uRays[index].vfReflectiveFactor *= tx_TextureColor.xyz;
				}
			}					
		}
		g_uRays[index].iTriangleId = iTriangleId;		
		g_uAccumulation[index].xyz += g_fFactorAct*fFactor.xyz*fAmbientLight.xyz;
	}
	else if (iTriangleId==-1)
	{
		//Environment mapping can go here but there is a catch because the last reflect will be black
		//(could be solved by placing a last dispatch doing environment mapping)
		if (g_iEnvMappingFlag){
			g_uAccumulation[index].xyz += g_fFactorAct*fFactor.xyz*g_sEnvironmentTx.SampleLevel(g_ssSampler, g_uRays[index].vfDirection.xyz,0).xyz;//texture environment mapping
		} else {
			g_uAccumulation[index].xyz += g_fFactorAct*fFactor.xyz;
		}
		g_uRays[index].iTriangleId = -2;
	}

	g_uRays[index] = g_uRays[index];
	g_uIntersections[index] = g_uIntersections[index];
	
	// Apply color to texture
	//g_uAccumulation[index] += fColor.xyz*g_fFactorAct;
	g_uResultTexture[DTid.xy] = g_uAccumulation[index];
}