#include "V8PCH.h"
#include "JavascriptIsolate.h"
#include "JavascriptContext.h"
#include "JavascriptComponent.h"
#include "Config.h"
#include "Translator.h"
#include "Exception.h"

#include "JavascriptIsolate_Private.h"
#include "JavascriptContext_Private.h"

using namespace v8;

DEFINE_LOG_CATEGORY(Javascript);

UJavascriptIsolate::UJavascriptIsolate(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UJavascriptIsolate::Init(bool bIsEditor)
{
	const bool bIsClassDefaultObject = IsTemplate(RF_ClassDefaultObject);
	if (!bIsClassDefaultObject)
	{
		JavascriptIsolate = TSharedPtr<FJavascriptIsolate>(FJavascriptIsolate::Create(bIsEditor));
	}
}

void UJavascriptIsolate::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UJavascriptIsolate* This = CastChecked<UJavascriptIsolate>(InThis);

	if (This->JavascriptIsolate.IsValid())
	{
		Collector.AllowEliminatingReferences(false);
		
		This->JavascriptIsolate->AddReferencedObjects(This, Collector);		

		Collector.AllowEliminatingReferences(true);
	}		
	
	Super::AddReferencedObjects(This, Collector);
}

void UJavascriptIsolate::BeginDestroy()
{
	const bool bIsClassDefaultObject = IsTemplate(RF_ClassDefaultObject);
	if (!bIsClassDefaultObject)
	{
		JavascriptIsolate.Reset();
	}

	Super::BeginDestroy();
}

UJavascriptContext* UJavascriptIsolate::CreateContext()
{
	return NewObject<UJavascriptContext>(this);
}

void UJavascriptIsolate::GetHeapStatistics(FJavascriptHeapStatistics& Statistics)
{
	v8::HeapStatistics stats;

	if (JavascriptIsolate.IsValid())
	{
		JavascriptIsolate->isolate_->GetHeapStatistics(&stats);

		Statistics.TotalHeapSize = stats.total_heap_size();
		Statistics.TotalHeapSizeExecutable = stats.total_heap_size_executable();
		Statistics.TotalPhysicalSize = stats.total_physical_size();
		Statistics.TotalAvailableSize = stats.total_available_size();
		Statistics.UsedHeapSize = stats.used_heap_size();
		Statistics.HeapSizeLimit = stats.heap_size_limit();
		Statistics.MallocedMemory = stats.malloced_memory();
		Statistics.bDoesZapGarbage = !!stats.does_zap_garbage();
	}
}

UJavascriptContext::UJavascriptContext(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	const bool bIsClassDefaultObject = IsTemplate(RF_ClassDefaultObject);
	if (!bIsClassDefaultObject)
	{
		auto Isolate = Cast<UJavascriptIsolate>(GetOuter());
		JavascriptContext = TSharedPtr<FJavascriptContext>(FJavascriptContext::Create(Isolate->JavascriptIsolate,Paths));

		Expose("Context", this);

		SetContextId(GetName());
	}	
}

void UJavascriptContext::SetContextId(FString Name)
{
	ContextId = MakeShareable(new FString(Name));
}

void UJavascriptContext::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UJavascriptContext* This = CastChecked<UJavascriptContext>(InThis);

	if (This->JavascriptContext.IsValid())
	{
		Collector.AllowEliminatingReferences(false);

		This->JavascriptContext->AddReferencedObjects(This, Collector);

		Collector.AllowEliminatingReferences(true);
	}

	Super::AddReferencedObjects(This, Collector);
}

void UJavascriptContext::Expose(FString Name, UObject* Object)
{
	if (JavascriptContext.IsValid())
	{
		JavascriptContext->Expose(Name, Object);
	}
}

FString UJavascriptContext::GetScriptFileFullPath(FString Filename)
{
	if (JavascriptContext.IsValid())
	{
		return JavascriptContext->GetScriptFileFullPath(Filename);
	}
	return TEXT("");
}

FString UJavascriptContext::ReadScriptFile(FString Filename)
{
	if (JavascriptContext.IsValid())
	{
		return JavascriptContext->ReadScriptFile(Filename);
	}
	return TEXT("");
}

void UJavascriptContext::RunFile(FString Filename)
{
	if (JavascriptContext.IsValid())
	{
		JavascriptContext->Public_RunFile(Filename);
	}
}

FString UJavascriptContext::RunScript(FString Script, bool bOutput)
{
	if (JavascriptContext.IsValid())
	{
		return JavascriptContext->Public_RunScript(Script, bOutput);
	}
	return TEXT("");
}

void UJavascriptContext::RequestV8GarbageCollection()
{
	if (JavascriptContext.IsValid())
	{
		JavascriptContext->RequestV8GarbageCollection();
	}
}

void UJavascriptContext::FindPathFile(FString TargetRootPath, FString TargetFileName, TArray<FString>& OutFiles)
{
	if (JavascriptContext.IsValid())
	{
		JavascriptContext->FindPathFile(TargetRootPath, TargetFileName, OutFiles);
	}
}

void UJavascriptContext::CreateInspector(int32 Port)
{
	if (JavascriptContext.IsValid())
	{
		JavascriptContext->CreateInspector(Port);
	}
}

void UJavascriptContext::DestroyInspector()
{
	if (JavascriptContext.IsValid())
	{
		JavascriptContext->DestroyInspector();
	}	
}

bool UJavascriptContext::IsDebugContext() const
{
	if (JavascriptContext.IsValid())
	{
		return JavascriptContext->IsDebugContext();
	}
	return false;
}

bool UJavascriptContext::WriteAliases(FString Filename)
{
	if (JavascriptContext.IsValid())
	{
		return JavascriptContext->WriteAliases(Filename);
	}
	return false;
}

bool UJavascriptContext::WriteDTS(FString Filename, bool bIncludingTooltip)
{
	if (JavascriptContext.IsValid())
	{
		return JavascriptContext->WriteDTS(Filename, bIncludingTooltip);
	}
	return false;
}

bool UJavascriptContext::HasProxyFunction(UObject* Holder, UFunction* Function)
{
	if (JavascriptContext.IsValid())
	{
		return JavascriptContext->HasProxyFunction(Holder, Function);
	}
	return false;
}

bool UJavascriptContext::CallProxyFunction(UObject* Holder, UObject* This, UFunction* FunctionToCall, void* Parms)
{
	if (JavascriptContext.IsValid())
	{
		return JavascriptContext->CallProxyFunction(Holder, This, FunctionToCall, Parms);
	}
	return false;
}

void UJavascriptContext::BeginDestroy()
{
	Super::BeginDestroy();

	JavascriptContext.Reset();
	ContextId.Reset();
}
