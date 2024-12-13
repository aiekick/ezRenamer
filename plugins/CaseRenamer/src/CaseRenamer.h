#pragma once

#include <apis/ezRenamerPluginApi.h>

class CaseRenamer : public rnm::PluginInterface {
public:
	CaseRenamer();
    virtual ~CaseRenamer() = default;
	bool Init() override;
    void Unit() override;
    uint32_t GetMinimalCashMeVersionSupported() const override;
	uint32_t GetVersionMajor() const override;
	uint32_t GetVersionMinor() const override;
	uint32_t GetVersionBuild() const override;
    std::string GetName() const override;
    std::string GetAuthor() const override;
    std::string GetVersion() const override;
    std::string GetContact() const override;
	std::string GetDescription() const override;
    std::vector<rnm::PluginModuleInfos> GetModulesInfos() const override;
    rnm::PluginModulePtr CreateModule(const std::string& vPluginModuleName) override;
    std::vector<rnm::PluginSettingsConfig> GetSettings() const override;
};