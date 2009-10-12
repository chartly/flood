/************************************************************************
*
* vaporEngine (2008-2009)
*
*	<http://www.portugal-a-programar.org>
*
************************************************************************/

#pragma once

namespace vapor {

//-----------------------------------//

std::vector<std::string>& split(const std::string &s, char delim, 
								std::vector<std::string> &elems);

//-----------------------------------//

std::vector<std::string> split(const std::string &s, char delim);

//-----------------------------------//

} // end namespace