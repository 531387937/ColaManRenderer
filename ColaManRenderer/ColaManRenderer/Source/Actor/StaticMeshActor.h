#pragma once

#include "Actor/Actor.h"
#include "Component/MeshComponent.h"

class CStaticMeshActor : public CActor
{
public:
	CStaticMeshActor(const std::string& Name);

	~CStaticMeshActor();

	void SetMesh(std::string MeshName);

	void SetMaterialInstance(std::string MaterialInstanceName);

	void SetTextureScale(const SVector2& Scale);

	//void SetUseSDF(bool bUseSDF);

private:
	CMeshComponent* StaticMeshComponent = nullptr;
};
