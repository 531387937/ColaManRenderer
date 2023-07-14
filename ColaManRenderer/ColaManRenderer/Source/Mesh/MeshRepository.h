#pragma once

#include <unordered_map>
#include <string>
#include "Mesh.h"
//#include "FbxLoader.h"

class CMeshRepository
{
public:
    CMeshRepository();

    static CMeshRepository& Get();

    void Load();

    void Unload();

public:
    std::unordered_map<std::string /*MeshName*/, CMesh> MeshMap;

private:
    //std::unique_ptr<TFbxLoader> FbxLoader;
};
