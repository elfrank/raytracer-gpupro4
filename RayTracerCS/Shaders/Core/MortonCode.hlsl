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

int Part1By2( int n )
{
	n = ( n ^ ( n << 16 ) ) & 0xff0000ff;
	n = ( n ^ ( n << 8 ) ) & 0x0300f00f;
	n = ( n ^ ( n << 4 ) ) & 0x030c30c3;
	n = ( n ^ ( n << 2 ) ) & 0x09249249;
	return n;
}

int ComputeMortonCode( int3 viCoord )
{
	return int( ( Part1By2( viCoord.z) << 2 ) + ( Part1By2( viCoord.y ) << 1 ) + ( Part1By2( viCoord.x ) ) );
}