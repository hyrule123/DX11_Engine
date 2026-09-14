#include "Engine/Core/pch.h"
#include "ComputePass.h"

#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/GPU/Shader/ComputeShader.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	ComputePass::ComputePass(const HashedStringView& concrete_class_name)
		: Super(concrete_class_name)
	{}

	ComputePass::~ComputePass()
	{}
	void ComputePass::Init()
	{
		Super::Init();

		compute_shader_ = LoadComputeShader();

		ASSERT(compute_shader_);
	}
	void ComputePass::Execute(ID3D11DeviceContext* context)
	{
		if (compute_shader_ == nullptr || compute_shader_->IsReady() == false)
		{
			ASSERT(false);
			return;
		}

		std::array<UINT, 3> thread_count = GetThreadCount();
		std::array<UINT, 3> threads_per_group = compute_shader_->GetThreadGroupSize();
		std::array<UINT, 3> group_count;	// Dispatch에 전달할 그룹 수
		for (size_t i = 0; i < 3; ++i)
		{
			if (threads_per_group[i] == 0)
			{
				ASSERT_F(false, "Group size of ComputeShader was 0.");
				return;
			}
			if (thread_count[i] == 0)
			{
				ASSERT_F(false, "Thread count of ComputePass was 0.");
				return;
			}

			// 올림 계산 관용구
			// 딱 맞아떨어질 때는 정확히 나누어 떨어지지만, 나머지가 있으면 한 그룹 더 필요하기 때문
			// thread = 100개, group = 8개 -> (100 + 7) / 8 = 13개 그룹 필요
			group_count[i] = (thread_count[i] + threads_per_group[i] - 1) / threads_per_group[i];
		}

		compute_shader_->Bind(context);
		BindResources(context);
		context->Dispatch(group_count[0], group_count[1], group_count[2]);
		UnbindResources(context);
		compute_shader_->Unbind(context);
	}
}

