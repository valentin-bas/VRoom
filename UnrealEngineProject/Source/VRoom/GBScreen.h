

#pragma once

#include "GameFramework/Actor.h"
#include <GBRenderer.h>

#include "GBScreen.generated.h"

/**
 * 
 */

namespace tgb
{
	class Emulator;
}

UCLASS()
class VROOM_API AGBScreen : public AActor
{
private:

	GENERATED_UCLASS_BODY()

	class CGBFramebuffer : public tgb::GBRenderer
	{
	public:
		static const int	kFrameBufferWidth = 160;
		static const int	kFrameBufferHeight = 144;

	private:
		uint8			m_Framebuffer[kFrameBufferWidth * kFrameBufferHeight * 4];
		bool			m_RefreshScreen;
		int				m_padstate;

	public:
		CGBFramebuffer();

		bool	UniqueNeedRefresh();

		virtual void	refresh() override;
		virtual void	render_screen(byte *buf, int width, int height, int depth) override;
		virtual int		check_pad() override;

		uint8*	Data() { return m_Framebuffer; }
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMeshComponents) TSubobjectPtr<UStaticMeshComponent> screen;
	UPROPERTY(EditDefaultsOnly, Category = Materials) UMaterialInterface *MasterMaterialRef;
	UPROPERTY() UTexture2D *DynamicTexture;
	UPROPERTY() UMaterialInstanceDynamic* DynamicMaterial;

	CGBFramebuffer	m_Framebuffer;
	tgb::Emulator*	m_Emulator;

protected:
	virtual void Tick(float dt) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

public:
	UFUNCTION(BlueprintCallable, Category = "InputsBinding") void UpButtonChangeState(bool pressed);
	UFUNCTION(BlueprintCallable, Category = "InputsBinding") void DownButtonChangeState(bool pressed);
	UFUNCTION(BlueprintCallable, Category = "InputsBinding") void LeftButtonChangeState(bool pressed);
	UFUNCTION(BlueprintCallable, Category = "InputsBinding") void RightButtonChangeState(bool pressed);
	UFUNCTION(BlueprintCallable, Category = "InputsBinding") void AButtonChangeState(bool pressed);
	UFUNCTION(BlueprintCallable, Category = "InputsBinding") void BButtonChangeState(bool pressed);
	UFUNCTION(BlueprintCallable, Category = "InputsBinding") void SelectButtonChangeState(bool pressed);
	UFUNCTION(BlueprintCallable, Category = "InputsBinding") void StartButtonChangeState(bool pressed);
	
};
