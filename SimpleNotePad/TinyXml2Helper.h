﻿#pragma once
#include "tinyxml2.h"
#include <string>
#include <functional>

class CTinyXml2Helper
{
public:

    static void LoadXmlFile(tinyxml2::XMLDocument& doc, const wchar_t* file_path);

    static void IterateChildNode(tinyxml2::XMLElement* ele, std::function<void(tinyxml2::XMLElement*)> fun);
};
