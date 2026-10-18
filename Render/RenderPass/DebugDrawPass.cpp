#include "Engine/Core/pch.h"
#include "DebugDrawPass.h"

#include <Engine/Manager/TimeManager.h>

#include <Engine/Render/RenderView.h>

#include <Engine/Resource/GPU/State/DepthStencilState.h>
#include <Engine/Resource/GPU/Buffer/StructuredBuffer.h>
#include <Engine/Resource/GPU/PipelineState.h>
#include <Engine/Resource/GPU/Mesh.h>
#include <Engine/Resource/GPU/Vertex.h>


namespace engine
{
	namespace vertex::debug_draw
	{
		using Vertex = DebugVSInput;

		constexpr auto kInputLayoutDescs = []() {
			std::array<D3D11_INPUT_ELEMENT_DESC, 1> descs{};
			//VS의 입력 구조체 매개변수 이름과 일치해야 함
			descs[0].SemanticName = "POSITION";

			//POSITION0을 의미 (뒤에 숫자가 없을 때 기본값 0)
			descs[0].SemanticIndex = 0;

			//데이터 포맷
			descs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

			//InputSlot: 0번 정점 버퍼 슬롯을 사용합니다.
			descs[0].InputSlot = 0;

			//AlignedByteOffset: 구조체 시작점으로부터 몇 번째부터 읽어야 하는지
			descs[0].AlignedByteOffset = 0;

			//정점별로 데이터를 보낼것인지 인스턴스별로 보낼것인지
			descs[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

			//인스턴스 별로 보낼 때 인스턴스 별로 몇단계씩 올릴 것인지
			//e.g) 2 -> 0, 2, 4, 8, ...
			descs[0].InstanceDataStepRate = 0;

			return descs;
			}();
	}

	DebugDrawPass::DebugDrawPass()
		: Super(DebugDrawPass::kClassConcreteName, render_pass::ID::DebugDraw)
		, debug_rect_mesh_(nullptr)
		, debug_circle_mesh_(nullptr)
		, debug_rect_data_()
		, debug_circle_data_()
		, debug_buffer_(nullptr)
		, debug_shader_set_(nullptr)
	{}
	DebugDrawPass::~DebugDrawPass()
	{}
	void DebugDrawPass::Init()
	{
		Super::Init();

		debug_buffer_ = std::make_unique<StructuredBuffer>();

		bool result = debug_buffer_->CreateDynamicBuffer<DebugDrawPerInstanceData>(512);
		ASSERT(result);

		//Debug DSS
		std::shared_ptr<DepthStencilState> dss = EntityManager::CreateEntity<DepthStencilState>();

		// 1. Depth Stencil Desc 구조체 선언 및 초기화
		D3D11_DEPTH_STENCIL_DESC dss_desc = {};

		// ------------------------------------------------------------------
		// 1. 깊이(Depth) 테스트 설정
		// ------------------------------------------------------------------
		dss_desc.DepthEnable = FALSE;                           // 깊이 테스트 OFF

		// ------------------------------------------------------------------
		// 2. 스텐실(Stencil) 테스트 설정 (일반적인 기본 렌더링에서는 끕니다)
		// ------------------------------------------------------------------
		dss_desc.StencilEnable = FALSE;                        // 스텐실 테스트를 끕니다.

		// 2. State 객체 생성
		if (false == dss->Create(dss_desc))
		{
			CHECK(false);
		}

#pragma region RECT MESH
		{
			debug_rect_mesh_ = EntityManager::CreateEntity<Mesh>();

			//Mesh
			//VERTEX BUFFER
			std::vector<vertex::debug_draw::Vertex> vertices;
			vertices.resize(4);
			vertices[0].position = { -0.5f, 0.5f, 0.0f };
			vertices[1].position = { 0.5f, 0.5f, 0.0f };
			vertices[2].position = { 0.5f, -0.5f, 0.0f };
			vertices[3].position = { -0.5f, -0.5f, 0.0f };

			AABB3D bounds;
			for (const auto& v : vertices)
			{
				bounds.Encapsulate(v.position);
			}

			bool result = debug_rect_mesh_->CreateVertexBuffer<vertex::debug_draw::Vertex>(vertices, bounds);
			ASSERT(result);

			//INDEX BUFFER
			std::vector<uint16_t> indices;
			indices.push_back(0u);
			indices.push_back(1u);
			indices.push_back(2u);
			indices.push_back(3u);
			indices.push_back(0u);
			result = debug_rect_mesh_->CreateIndexBuffer(indices, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, {});
			ASSERT(result);
		}
#pragma endregion // RECT MESH
#pragma region CIRCLE MESH
		{
			debug_circle_mesh_ = EntityManager::CreateEntity<Mesh>();

			//Mesh
			std::vector<vertex::debug_draw::Vertex> vertices;
			std::vector<uint16_t> indices;

			vertex::debug_draw::Vertex v;
			//v.position = { 0.0f, 0.0f, 0.0f };	//중심점
			//vertices.push_back(v);

			//32개 (vertex: 32개 index 0 ~ 31)
			for (uint32_t i = 0; i <= 31; ++i)
			{
				float angle = (float)i / 32.0f * XM_2PI;
				float x = cosf(angle) * 0.5f;
				float y = sinf(angle) * 0.5f;
				vertices.push_back({ {x, y, 0.0f} });
				indices.push_back((uint16_t)i);
			}
			indices.push_back(0u);	//마지막에 0번으로 돌아가서 닫힌 원 만들기

			AABB3D bounds;
			for (const auto& v : vertices)
			{
				bounds.Encapsulate(v.position);
			}
			debug_circle_mesh_->CreateVertexBuffer<vertex::debug_draw::Vertex>(vertices, bounds);
			debug_circle_mesh_->CreateIndexBuffer(indices, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, {});
		}
#pragma endregion // CIRCLE MESH

#pragma region // GRAPHICS SHADER SET
		debug_shader_set_ = std::make_unique<PipelineState>();

		debug_shader_set_->SetInstanceDataStride(sizeof(DebugDrawPerInstanceData));
		//Shaders
		debug_shader_set_->SetVertexShader("Shader/DebugDraw_VS.cso");
		debug_shader_set_->CreateInputLayout(vertex::debug_draw::kInputLayoutDescs);
		debug_shader_set_->SetPixelShader("Shader/DebugDraw_PS.cso");
		debug_shader_set_->SetDepthStencilState(dss);
#pragma endregion // GRAPHICS SHADER SET
	}

	void DebugDrawPass::Execute(const RenderContext& ctx)
	{
		if (debug_rect_data_.empty() && debug_circle_data_.empty())
		{
			return;
		}

		//Shader Set Bind
		debug_shader_set_->Bind(ctx.dc);

		//Debug Draw용 버퍼 - 사이즈 부족 시 확장
		size_t max_size = std::max(debug_rect_data_.size(), debug_circle_data_.size());
		if (max_size > debug_buffer_->GetElementCount())
		{
			//벡터랑 버퍼 확장 타이밍 맞추기 위해 capacity() 사용
			bool result = debug_buffer_->Reserve((uint32_t)max_size);
			ASSERT(result);
		}


#pragma region //Debug Rect Draw
		float dt = TimeManager::GetInst().GetDeltaTime();
		if (false == debug_rect_data_.empty())
		{
			std::span debug_rect_span(debug_rect_data_);
			debug_buffer_->Upload(ctx.dc, debug_rect_span);
			debug_buffer_->BindSRV(ctx.dc, ShaderStage::Vertex | ShaderStage::Pixel, REG_T_INSTANCE_BUFFER);

			//Mesh Draw
			debug_rect_mesh_->Bind(ctx.dc);
			debug_rect_mesh_->Draw(ctx.dc, {.instance_start = 0, .instance_count = (uint32_t)debug_rect_data_.size(), .submesh_index = 0});

			// 순회 돌면서 dt 감소 및 음수가 된 값들은 제거
			for (int32_t i = (int32_t)debug_rect_data_.size() - 1; i >= 0; --i)
			{
				debug_rect_data_[i].left_time -= dt;
				if (debug_rect_data_[i].left_time <= 0.0f)
				{
					debug_rect_data_[i] = debug_rect_data_.back();
					debug_rect_data_.pop_back();
				}
			}
		}
#pragma endregion //Debug Rect Draw

#pragma region //Debug Circle Draw
		if (false == debug_circle_data_.empty())
		{
			std::span debug_circle_span(debug_circle_data_);
			debug_buffer_->Upload(ctx.dc, debug_circle_span);
			debug_buffer_->BindSRV(ctx.dc, ShaderStage::Vertex | ShaderStage::Pixel, REG_T_INSTANCE_BUFFER);

			//Mesh Draw
			debug_circle_mesh_->Bind(ctx.dc);
			debug_circle_mesh_->Draw(ctx.dc, {.instance_start = 0, .instance_count = (uint32_t)debug_circle_data_.size(), .submesh_index = 0});

			// 순회 돌면서 dt 감소 및 음수가 된 값들은 제거
			for (int32_t i = (int32_t)debug_circle_data_.size() - 1; i >= 0; --i)
			{
				debug_circle_data_[i].left_time -= dt;
				if (debug_circle_data_[i].left_time <= 0.0f)
				{
					debug_circle_data_[i] = debug_circle_data_.back();
					debug_circle_data_.pop_back();
				}
			}
		}
#pragma endregion //Debug Circle Draw
	}

}