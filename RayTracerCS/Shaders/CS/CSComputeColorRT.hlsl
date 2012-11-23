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
[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]
void CSComputeColor(uint3 DTid : SV_DispatchThreadID, uint GIndex : SV_GroupIndex)
{
	
	//return;
	// Calculate 1D index of the current thread
	const unsigned int index = DTid.y * N + DTid.x;

	// Get the triangle ID stored in the current pixel
	const int iTriangleId = g_uIntersections[index].iTriangleId;	

	// If Environment Mapping has already been applied to this ray, return.
	if(iTriangleId < g_iEnvMappingFlag) 
	//if(iTriangleId < -1) 
	{
		if(iTriangleId > (-2)) g_uResultTexture[DTid.xy] = 1.f;
		return;
	}

	// Init variables
	
	float4 vfFinalColor = float4(0.f,0.f,0.f,1.f);
	float4 vfDiffuse = float4(0.f, 0.f, 0.f, 1.f);
	float4 vfSpecular = float4(0.f,0.f,0.f,1.f);
	float3 vfFactor = g_uRays[index].vfReflectiveFactor;

	float3 vfNormal = 1.f;
	float3 vfNormalNM = 1.f;

	// If ray hits something: Apply iMaterial.
	// Otherwise: Apply environment texture and kill the ray (because the
	// intersection is null).
	if (iTriangleId >= 0)
	{
		const int currentMaterial = g_sPrimitivesData[iTriangleId].iMaterialId;

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
			vfNormal = normalize(cross(Edge1, Edge2));
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
		
		// Point normals to face ray source
		bool inverseNormal=false;
		if (dot(vfNormalNM,g_uRays[index].vfDirection) > 0.f)
		{
			vfNormalNM = -vfNormalNM;
			inverseNormal=true;
		}
		
		// Generate hit point and light dir
		float3 vfHitPoint = g_uRays[index].vfOrigin + 
			g_uIntersections[index].fT * g_uRays[index].vfDirection;
		float3 vfLightDir = vfHitPoint - g_vfLightPosition;
		const float fLightDistance = length(vfLightDir);
		vfLightDir /= fLightDistance;
		
		// Ray reflection
		const float3 vfNewDirection = normalize(
			reflect(g_uRays[index].vfDirection,vfNormalNM));

		// Check if light source and ray origin are in the same siIde
		const float lightDirSign = dot(vfNormal,vfLightDir);
		const float rayDirSign = dot(vfNormal,g_uRays[index].vfDirection);
		if ((lightDirSign*rayDirSign)>0.f)
		{
			Ray r = g_uRays[index];
			r.vfOrigin = vfHitPoint;
			r.vfDirection = -vfLightDir.xyz;
			r.iTriangleId = iTriangleId;
			r.fMaxT = fLightDistance;

			// Cast shadows
			if(g_bIsShadowOn)
			{
				int tr = 0;
				if(g_iAccelerationStructure == 0) tr = BVH_IntersectP(r).iTriangleId;
				else if(g_iAccelerationStructure == 9) tr = LBVH_IntersectP(r, g_uIntersections[index].iRoot).iTriangleId;
				if(tr < 0)
				{
					// Specular light term
					const float shininess = -dot(vfLightDir, vfNewDirection);
					vfSpecular = pow(max (0.f, shininess), 49);
					// Diffuse light term
					vfDiffuse = max(0,-dot(vfNormalNM, vfLightDir.xyz));
				}
			}
			else
			{
				// Specular light term
				vfSpecular = pow(max (0.f, -dot(vfLightDir.xyz, vfNewDirection)), 49);
				// Diffuse light term
				vfDiffuse = max(0,-dot(vfNormalNM, vfLightDir.xyz));
			}
		}
		// END IF	
		
		//unsigned int texturesOn = 0;
		// Apply Lambert iMaterial

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

		// Calculate color for the current pixel
		const float4 vfDiffuseColor = (fAmbientLight + vfDiffuse*tx_TextureColor.w) * tx_TextureColor;
		const float4 vfSpecularColor = vfSpecular * (1.0-tx_TextureColor.w) * tx_SpecularColor;
		vfFinalColor = float4(vfDiffuseColor + vfSpecularColor);		
		vfFinalColor *= float4(vfFactor, 1.f);
		
		// Ray bounce		
		g_uRays[index].vfOrigin = vfHitPoint;
		float2 indexasfloat = float2(DTid.x,DTid.y)/1024.0f;
		float4 randomDir = g_sRandomTx.SampleLevel(g_ssSampler, indexasfloat+float2(g_fRandX,g_fRandY), 0);
		float Preflection = g_sMaterials[currentMaterial].fAlpha;
		if (Preflection>=randomDir.w)
		{
			g_uRays[index].vfReflectiveFactor *= tx_SpecularColor.xyz;
			g_uRays[index].vfDirection = vfNewDirection;				// Ray Tracing
		}
		else
		{
			//refraction
			float refindex = fAmbientLight.a;
			if (!inverseNormal)
			{
				refindex=1.0/refindex;			
			}
			float3 refractdir = refract(g_uRays[index].vfDirection , vfNormalNM, refindex );
			float refnorm = length(refractdir);
			if (refnorm<0.000001) {
			    //total inner reflection
			    g_uRays[index].vfDirection = vfNewDirection;				
			}
			else {
			    g_uRays[index].vfDirection = refractdir/refnorm;
				g_uRays[index].vfReflectiveFactor *= tx_TextureColor.xyz;
			}					
		}
		g_uRays[index].iTriangleId = iTriangleId;		
	}
	else
	{
		const float4 tx_Environment = g_sEnvironmentTx.SampleLevel(
				g_ssSampler, g_uRays[index].vfDirection.xyz,0);

		//Environment mapping
		vfFinalColor.xyz = vfFactor.xyz * tx_Environment.xyz;	
		// This indicates that the Environment Mapping has
		// been applied to the current pixel.
		g_uRays[index].iTriangleId = -2;
		//g_uIntersections[index].iTriangleId = -2;
		
	}
	// END IF
	
	// Apply color to texture	
	//vfFinalColor.x += (float)(g_uIntersections[index].iVisitedNodes&65535)*0.02;
	//vfFinalColor.y += (float)(g_uIntersections[index].iVisitedNodes>>16)*0.02;
	//float maxcolor = vfFinalColor.y;
	//vfFinalColor.x = 1.0 - maxcolor;
	//vfFinalColor.y = 1.0;
	//vfFinalColor.z = 1.0 - maxcolor;//(float)(g_uIntersections[index].iVisitedNodes>>16)*0.00001;
	g_uAccumulation[index] += vfFinalColor;
	g_uResultTexture[DTid.xy] = g_uAccumulation[index];
	//g_uResultTexture[DTid.xy] = float4(g_uIntersections[index].fT,g_uIntersections[index].fU,g_uIntersections[index].fV,1.f);
	//float grayScale = (vfNormalNM.x+vfNormalNM.y+vfNormalNM.z)/3;
	//g_uResultTexture[DTid.xy] = float4(grayScale,grayScale,grayScale,1.f);
	//g_uResultTexture[DTid.xy] = float4(g_uRays[index].vfDirection,1.f);
	//g_uResultTexture[DTid.xy] = float4(iTriangleId*0.000008,iTriangleId*0.000008,iTriangleId*0.000008,1);
}