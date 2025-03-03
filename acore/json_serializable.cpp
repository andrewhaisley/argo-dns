// 
// Copyright 2025 Andrew Haisley
// 
// This program is free software: you can redistribute it and/or modify it under the terms 
// of the GNU General Public License as published by the Free Software Foundation, either 
// version 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with this program. 
// If not, see https://www.gnu.org/licenses/.
// 
#include "json_serializable.hpp"

using namespace std;
using namespace adns;

json_serializable::json_serializable() : m_json_object(nullptr)
{
}

json_serializable::~json_serializable()
{
}

json json_serializable::to_json() const
{
    m_json_object = make_shared<json>(json::object_e);
    json_serialize();
    return *m_json_object;
}

void json_serializable::from_json(const json &j) 
{
    m_json_object = make_shared<json>(j);
    json_unserialize();
}
