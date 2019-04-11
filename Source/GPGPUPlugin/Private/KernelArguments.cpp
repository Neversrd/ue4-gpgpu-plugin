#include "KernelArguments.h"

UKernelArguments* UKernelArguments::CreateKernelArguments(UObject* WorldContextObject) {
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UKernelArguments* tempObject = NewObject<UKernelArguments>();

	return tempObject;
}

void UKernelArguments::ParseKernelArgumentsFromStructure(UProperty* kernelArgumentStructure, UKernelArguments* kernelArgs) {
    check(0);
}

boost::variant<double, long long> UKernelArguments::SubParseProperty(UProperty* Property, void* ValuePtr) {
    if (UNumericProperty *NumericProperty = Cast<UNumericProperty>(Property))
    {
        if (NumericProperty->IsFloatingPoint())
        {
            return NumericProperty->GetFloatingPointPropertyValue(ValuePtr);
        }
        else if (NumericProperty->IsInteger())
        {
            return NumericProperty->GetSignedIntPropertyValue(ValuePtr);
        }
    }
    return (double)0.0f;
}

void UKernelArguments::ParseProperty(UProperty* Property, void* ValuePtr, UKernelArguments* kernelArgs)
{        
    if (UNumericProperty *NumericProperty = Cast<UNumericProperty>(Property))
    {
        if (NumericProperty->IsFloatingPoint())
        {
            kernelArgs->Arguments.push_back(NumericProperty->GetFloatingPointPropertyValue(ValuePtr));
        }
        else if (NumericProperty->IsInteger())
        {
            kernelArgs->Arguments.push_back(NumericProperty->GetSignedIntPropertyValue(ValuePtr));
        }
    }
    if (UArrayProperty* ArrayProperty = Cast<UArrayProperty>(Property))
    {      
        std::vector<boost::variant<double, long long>> subArguments;
        FScriptArrayHelper Helper(ArrayProperty, ValuePtr);
        
        for (int32 i = 0, n = Helper.Num(); i<n; ++i)
        {    
            subArguments.push_back(SubParseProperty(ArrayProperty->Inner, Helper.GetRawPtr(i)));
        }
        kernelArgs->Arguments.push_back(subArguments);
    }
}

void UKernelArguments::IterateThroughStructProperty(UStructProperty* StructProperty, void* StructPtr, UKernelArguments* kernelArgs) {
    UScriptStruct* Struct = StructProperty->Struct;
    for (TFieldIterator<UProperty> It(Struct); It; ++It)
    {
        UProperty* Property = *It;
        FString VariableName = Property->GetName();
        for (int32 ArrayIndex = 0; ArrayIndex < Property->ArrayDim; ArrayIndex++)
        {
            void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructPtr, ArrayIndex);
            ParseProperty(Property, ValuePtr, kernelArgs);
        }
    }
}