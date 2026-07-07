#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/Enum.h>

#include <array>

struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

namespace engine
{
    class GraphicsShaderSet;
    class Texture2D;

    class Material :
        public Resource
    {
        CLASS_INFO(Material, Resource)

    public:
        using Textures = std::array<s_ptr<Texture2D>, kMaxTextureCount>;

        Material();
        Material(const Material& other) = default;
        virtual ~Material() override;

        //고유 텍스처를 만들 때 싸용
        s_ptr<Material> Clone() const { 
            return std::make_shared<Material>(*this); 
        }

		bool IsReady(RenderPassOrder pass) const {
			return (bool)shader_sets_per_pass_[(size_t)pass];
		}

        bool SetShaderSet(const stdfs::path& shader_set_name, RenderPassOrder pass);
        void SetShaderSet(s_ptr<GraphicsShaderSet> shader_set, RenderPassOrder pass);
        bool BindShaderSet(ID3D11DeviceContext* context, RenderPassOrder pass);

        void BindTextures(ID3D11DeviceContext* context, ShaderStage::Flags stage_flag);

        bool SetTexture(const stdfs::path& texture_filepath, uint32 slot);
        void SetTexture(s_ptr<Texture2D> tex, uint32 slot);
        void SetTextures(const Textures& textures) {
            for (size_t i = 0; i < textures.size(); ++i)
            {
                SetTexture(textures[i], (uint32)i);
            }
        }

        size_t GetInstanceDataStride(RenderPassOrder pass) const;

    private:
        Textures textures_ = {};
        std::array<ID3D11ShaderResourceView*, kMaxTextureCount> srv_cache_ = {};

		std::array<s_ptr<GraphicsShaderSet>, (size_t)RenderPassOrder::kEND> 
            shader_sets_per_pass_ = {};
    };
}


