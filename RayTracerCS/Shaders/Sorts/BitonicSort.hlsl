//--------------------------------------------------------------------------------------
// File: ComputeShaderSort11.hlsl
//
// This file contains the compute shaders to perform GPU sorting using DirectX 11.
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#define BITONIC_BLOCK_SIZE 512
#define TRANSPOSE_BLOCK_SIZE 16

//--------------------------------------------------------------------------------------
// Bitonic Sort Compute Shader
//--------------------------------------------------------------------------------------
#ifdef MORTON_CODE
groupshared MortonCode shared_data[BLOCK_SIZE_X];
#elif defined (UINT)
groupshared unsigned int shared_data[BLOCK_SIZE_X];
#endif

#ifdef MORTON_CODE

#elif defined (UINT) || defined (INT)

#endif

[numthreads(BITONIC_BLOCK_SIZE, 1, 1)]
void CSBitonicSort( uint3 Gid : SV_GroupID, 
                  uint3 DTid : SV_DispatchThreadID, 
                  uint3 GTid : SV_GroupThreadID, 
                  uint GI : SV_GroupIndex )
{
    // Load shared g_uMortonCode
    shared_data[GI] = g_uMortonCode[DTid.x];
    GroupMemoryBarrierWithGroupSync();
    
    // Sort the shared g_uMortonCode
    for (unsigned int j = g_iLevel >> 1 ; j > 0 ; j >>= 1)
    {
#ifdef MORTON_CODE
		MortonCode result;
#elif defined (UINT)
		unsigned int result;
#endif

#ifdef MORTON_CODE
		if((shared_data[GI & ~j].iCode < shared_data[GI | j].iCode) == (bool)(g_iLevelMask & DTid.x))
#elif defined (UINT)
		if((shared_data[GI & ~j] < shared_data[GI | j]) == (bool)(g_iLevelMask & DTid.x))
#endif
		{
			result = shared_data[GI ^ j];
		}
		else
		{
			result = shared_data[GI];
		}
        GroupMemoryBarrierWithGroupSync();
        shared_data[GI] = result;
        GroupMemoryBarrierWithGroupSync();
    }
    
    // Store shared g_uMortonCode
    g_uMortonCode[DTid.x] = shared_data[GI];
	g_uPrimitivesId[DTid.x] = shared_data[GI].iId;
}

//--------------------------------------------------------------------------------------
// Matrix Transpose Compute Shader
//--------------------------------------------------------------------------------------
#ifdef MORTON_CODE
groupshared MortonCode transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];
#elif defined (UINT) || defined (INT)
groupshared unsigned int transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];
#endif

[numthreads(TRANSPOSE_BLOCK_SIZE, TRANSPOSE_BLOCK_SIZE, 1)]
void CSMatrixTranspose( uint3 Gid : SV_GroupID, 
                      uint3 DTid : SV_DispatchThreadID, 
                      uint3 GTid : SV_GroupThreadID, 
                      uint GI : SV_GroupIndex )
{
    transpose_shared_data[GI] = g_sInput[DTid.y * g_iWidth + DTid.x];
    GroupMemoryBarrierWithGroupSync();
    uint2 XY = DTid.yx - GTid.yx + GTid.xy;
    g_uMortonCode[XY.y * g_iHeight + XY.x] = transpose_shared_data[GTid.x * TRANSPOSE_BLOCK_SIZE + GTid.y];
	g_uPrimitivesId[XY.y * g_iHeight + XY.x] = transpose_shared_data[GTid.x * TRANSPOSE_BLOCK_SIZE + GTid.y].iId;
}

/*//#define BITONIC_BLOCK_SIZE 1024 -> BLOCK_SIZE_X
#define TRANSPOSE_BLOCK_SIZE 16

// ------------------------------------------
// Constant Buffer
// ------------------------------------------
cbuffer cbBitonicSort : register( b5 )
{
    unsigned int g_iLevel;
    unsigned int g_iLevelMask;
    unsigned int g_iWidth;
    unsigned int g_iHeight;
};

//--------------------------------------------------------------------------------------
// Bitonic Sort Compute Shader
//--------------------------------------------------------------------------------------
#ifdef MORTON_CODE
groupshared MortonCode shared_data[BLOCK_SIZE_X];
#elif defined (INT)
groupshared unsigned int shared_data[BLOCK_SIZE_X];
#endif

[numthreads(BLOCK_SIZE_X, 1, 1)]
void CSBitonicSort( uint3 Gid : SV_GroupID, 
                  uint3 DTid : SV_DispatchThreadID, 
                  uint3 GTid : SV_GroupThreadID, 
                  uint GI : SV_GroupIndex )
{
	

    // Load shared data
#ifdef MORTON_CODE
	shared_data[GI] = g_uMortonCode[DTid.x];
#elif defined (INT)
	shared_data[GI] = g_uMortonCode[DTid.x];
#endif	
    GroupMemoryBarrierWithGroupSync();
   
    // Sort the shared data
    for (unsigned int j = g_iLevel >> 1 ; j > 0 ; j >>= 1)
    {
#ifdef MORTON_CODE
		MortonCode result = shared_data[GI];
		if( (shared_data[GI & ~j].iCode > shared_data[GI | j].iCode) == ((bool)(g_iLevelMask & DTid.x)) )
		{
			result = shared_data[GI ^ j];
		}
#elif defined (INT)
		unsigned int result = ((shared_data[GI & ~j] < shared_data[GI | j]) == (bool)(g_iLevelMask & DTid.x))? shared_data[GI ^ j] : shared_data[GI];
#endif
        GroupMemoryBarrierWithGroupSync();
        shared_data[GI] = result;
        GroupMemoryBarrierWithGroupSync();
    }
    
    // Store shared data
#ifdef MORTON_CODE
	g_uMortonCode[DTid.x] = shared_data[GI];
	//g_uMortonCode[DTid.x].iCode = BLOCK_SIZE_X;
#elif defined (INT)
	g_uMortonCode[DTid.x] = shared_data[GI];
#endif
}

//--------------------------------------------------------------------------------------
// Matrix Transpose Compute Shader
//--------------------------------------------------------------------------------------
#ifdef MORTON_CODE
//groupshared MortonCode transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];
#elif defined (INT)
//groupshared int transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];
#endif

groupshared MortonCode transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];

[numthreads(TRANSPOSE_BLOCK_SIZE, TRANSPOSE_BLOCK_SIZE, 1)]
void CSMatrixTranspose( uint3 Gid : SV_GroupID, 
                      uint3 DTid : SV_DispatchThreadID, 
                      uint3 GTid : SV_GroupThreadID, 
                      uint GI : SV_GroupIndex )
{
	//if( DTid.x >= iNumPrimitives )
	{
		//g_uMortonCode[DTid.x].iCode = 20;
		//return;
	}
	//return;
	transpose_shared_data[GI] = g_sInput[DTid.y * g_iWidth + DTid.x];
    GroupMemoryBarrierWithGroupSync();
    uint2 XY = DTid.yx - GTid.yx + GTid.xy;
#ifdef MORTON_CODE
	g_uMortonCode[XY.y * g_iHeight + XY.x] = transpose_shared_data[GTid.x * TRANSPOSE_BLOCK_SIZE + GTid.y];
	//g_uMortonCode[XY.y * g_iHeight + XY.x] = g_sInput[GTid.x * TRANSPOSE_BLOCK_SIZE + GTid.y];
	//g_uMortonCode[XY.y * g_iHeight + XY.x].iCode = TRANSPOSE_BLOCK_SIZE;
#elif defined (INT)
	g_uMortonCode[XY.y * g_iHeight + XY.x] = transpose_shared_data[GTid.x * TRANSPOSE_BLOCK_SIZE + GTid.y];
#endif
}*/