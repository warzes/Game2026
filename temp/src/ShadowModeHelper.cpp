#include "RenderLib/ShadowModeHelper.hpp"
#include "RenderLib/ShadowManager.hpp"
#include "RenderLib/Renderer.hpp"

namespace RenderLib {

ShadowModeHelper::ShadowModeHelper(Renderer& renderer)
    : renderer_(renderer) {
    shadowMgr_ = std::make_shared<ShadowManager>();
    shadowMgr_->init();
}

void ShadowModeHelper::enableStandardShadows() {
    getShadowManager_()->setShadowMode(ShadowMode::Standard);
}

void ShadowModeHelper::enableRSM() {
    getShadowManager_()->setShadowMode(ShadowMode::RSM);
}

void ShadowModeHelper::enableIndividualMaps() {
    getShadowManager_()->setAtlasMode(AtlasMode::Individual);
}

void ShadowModeHelper::enableShadowAtlas(uint32_t lightCount, uint32_t resolution) {
    getShadowManager_()->setAtlasMode(AtlasMode::Atlas);
    getShadowManager_()->allocateAtlasSpace(lightCount, resolution);
}

bool ShadowModeHelper::isRSMEnabled() const {
    return getShadowManager_()->getShadowMode() == ShadowMode::RSM;
}

bool ShadowModeHelper::isAtlasEnabled() const {
    return getShadowManager_()->getAtlasMode() == AtlasMode::Atlas;
}

void ShadowModeHelper::renderAllShadows() {
    // Placeholder - would render shadows for all lights
    // Implementation depends on Renderer's light management
}

void ShadowModeHelper::updateShadowResolution(uint32_t newResolution) {
    // Update resolution for all lights
    // This would iterate through renderer's lights and update their shadow maps
}

ShadowManager* ShadowModeHelper::getShadowManager_() const {
    return shadowMgr_.get();
}

}  // namespace RenderLib
