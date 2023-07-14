#include "InputLayout.h"

void CInputLayoutManager::AddInputLayout(const std::string& Name,
                                         const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout)
{
    InputLayoutMap.insert({Name, InputLayout});
}

void CInputLayoutManager::GetInputLayout(const std::string Name,
                                         std::vector<D3D12_INPUT_ELEMENT_DESC>& OutInputLayout) const
{
    auto Iter = InputLayoutMap.find(Name);
    if (Iter == InputLayoutMap.end())
    {
        assert(0); //TODO
    }
    else
    {
        OutInputLayout = Iter->second;
    }
}
