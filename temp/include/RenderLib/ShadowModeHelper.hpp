#pragma once
#include <memory>

namespace RenderLib {

class ShadowManager;
class Renderer;

/**
 * @brief Helper class to integrate ShadowManager with Renderer
 * 
 * Provides convenience methods for shadow mode management
 */
class ShadowModeHelper {
public:
    explicit ShadowModeHelper(Renderer& renderer);

    // Shadow mode switching
    void enableStandardShadows();
    void enableRSM();

    // Atlas mode switching
    void enableIndividualMaps();
    void enableShadowAtlas(uint32_t lightCount = 4, uint32_t resolution = 1024);

    // Current mode queries
    bool isRSMEnabled() const;
    bool isAtlasEnabled() const;

    // Shadow rendering shortcuts
    void renderAllShadows();
    void updateShadowResolution(uint32_t newResolution);

private:
    Renderer& renderer_;
    std::shared_ptr<ShadowManager> shadowMgr_;

    ShadowManager* getShadowManager_() const;
};

}  // namespace RenderLib
