#include "Engine/pch.h"
#include "Mesh.h"


namespace engine
{
	Mesh::Mesh()
		: Resource("Mesh")
	{}
	Mesh::~Mesh()
	{}

	bool Mesh::LoadFromFile(const stdfs::path& path)
	{
		
		//아직은... 미구현(차후 fbx 임포트 시 구현 예정)
		return false;
	}
}

