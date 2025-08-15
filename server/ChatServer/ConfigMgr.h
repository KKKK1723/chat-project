#pragma once
#include"const.h"

struct SectionInfo
{
	SectionInfo() {};
	~SectionInfo() {
		_section_datas.clear();
	};

	std::string operator [] (const std::string& key)
	{
		if (_section_datas.find(key) == _section_datas.end())
		{
			return "";
		}
		return _section_datas[key];
	};
	std::map<std::string,std::string>_section_datas;
};

class ConfigMgr
{
public:
	~ConfigMgr()
	{
		_config_map.clear();
	}

	static ConfigMgr &Inst()
	{
		static ConfigMgr cfg_mgr;
		return cfg_mgr;
	}

	SectionInfo operator[](const std::string &key)
	{
		if (_config_map.find(key) == _config_map.end())
		{
			return SectionInfo();
		}
		return _config_map[key];
	};

	ConfigMgr(const ConfigMgr &t) = delete;
	ConfigMgr &operator=(const ConfigMgr &t) = delete;

private:
	ConfigMgr();
	std::map<std::string, SectionInfo> _config_map;
};
