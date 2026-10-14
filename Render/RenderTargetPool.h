#pragma once
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/DX11.h>

#include <Engine/Util/StringHashTable.h>

#include <memory>

namespace engine
{
    class Texture2D;
    class RenderTargetView;
    class DepthStencilView;

    // 크기 기준.
    // - Fixed: desc의 width/height 그대로. 해상도 변경에 반응하지 않는다.
    // - SwapChain: 스왑체인 해상도 × scale. OnScreenSizeChange에서 재조정된다.
    // (MainTarget 기준은 Scene이 메인 RT를 소유하게 될 때 추가)
    enum class RenderTargetSizeBase : uint8
    {
        Fixed,
        SwapChain,
    };

    // 아래 desc 구조체들은 지정 초기화({ .format = ... })를 쓰기 위해 집합체로 두고, 기본값을 헤더에 둔다.
    struct RenderTargetSize
    {
        RenderTargetSizeBase base = RenderTargetSizeBase::Fixed;
        uint32 width = 0;       // Fixed 전용
        uint32 height = 0;      // Fixed 전용
        float scale = 1.0f;     // SwapChain 전용

        bool operator==(const RenderTargetSize&) const = default;
    };

    struct ColorTargetDesc
    {
        RenderTargetSize size = {};
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        bool create_srv = true;
        bool create_uav = false;

        bool operator==(const ColorTargetDesc&) const = default;
    };

    struct DepthTargetDesc
    {
        RenderTargetSize size = {};
        DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        // 깊이 SRV(typeless 포맷 + 뷰 포맷 분리)는 필요해질 때 추가

        bool operator==(const DepthTargetDesc&) const = default;
    };

    // RenderManager가 값으로 소유하는 RT 저장소.
    // - 관리 단위는 텍스처(RenderTargetView / DepthStencilView) 하나. RTG 조합은 사용하는 쪽이 구성한다.
    // - 같은 이름으로 다시 요청하면 기존 텍스처를 반환한다. 종류(Color/Depth)나 desc가 다르면 실패.
    // - 수명은 참조 카운트로만 판단한다. 풀만 들고 있는 상태(use_count == 1)가
    //   kReleaseAfterUnusedFrames 프레임 연속되면 해제. 그 이상의 수명 관리는 사용하는 쪽의 책임.
    // - 크기 재조정은 객체를 유지한 채 내부 텍스처/뷰만 교체(in-place)하므로, 발급된 shared_ptr는 계속 유효하다.
    //   대신 RTV/SRV/UAV를 텍스처 객체 밖에 캐시하지 말 것(바인딩 시점에 꺼내 쓸 것).
    class RenderTargetPool
    {
    public:
        static constexpr uint32 kReleaseAfterUnusedFrames = 120;

        RenderTargetPool();
        ~RenderTargetPool();

        RenderTargetPool(const RenderTargetPool&) = delete;
        RenderTargetPool& operator=(const RenderTargetPool&) = delete;

        // 실패 시 nullptr
        std::shared_ptr<RenderTargetView> AcquireColor(const HashedStringView& name, const ColorTargetDesc& desc);
        std::shared_ptr<DepthStencilView> AcquireDepth(const HashedStringView& name, const DepthTargetDesc& desc);

        // 프레임당 1회 (RenderManager::FrameEnd). 미사용 프레임 수를 갱신하고 기준을 넘은 항목을 해제한다.
        void FrameEnd();

        // SwapChain 기준 항목을 재조정
        void OnScreenSizeChange(uint32 width, uint32 height);

    private:
        enum class Kind : uint8
        {
            Color,
            Depth,
        };

        struct Entry
        {
            Entry();

            Kind kind;
            ColorTargetDesc color_desc;     // kind == Color일 때만 유효
            DepthTargetDesc depth_desc;     // kind == Depth일 때만 유효
            uint32 unused_frames;           // 풀만 들고 있던 연속 프레임 수
            std::shared_ptr<Texture2D> texture;
        };

        // 이름으로 기존 항목을 찾아 종류를 검증한다. desc 비교는 호출부에서.
        // 반환: 항목이 없으면 nullptr(out_failed = false), 종류 불일치면 nullptr(out_failed = true)
        Entry* FindExisting(const HashedStringView& name, Kind kind, bool& out_failed);

        static std::shared_ptr<RenderTargetView> CreateColor(const ColorTargetDesc& desc);
        static std::shared_ptr<DepthStencilView> CreateDepth(const DepthTargetDesc& desc);

        static bool ResolveSize(const RenderTargetSize& size, uint32& out_width, uint32& out_height);
        static const RenderTargetSize& GetSize(const Entry& entry);

    private:
        StringHashTable<Entry> entries_;
    };
}