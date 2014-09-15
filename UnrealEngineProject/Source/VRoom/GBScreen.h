

#pragma once

#include "GameFramework/Actor.h"
#include "libvrgb/IGBScreenDrawable.h"
#include "GBScreen.generated.h"

/**
 * 
 */

namespace vrgb
{
	class VRGBEmulator;
	class Cartridge;
}

UCLASS()
class VROOM_API AGBScreen : public AActor
{
private:

	GENERATED_UCLASS_BODY()

	class CGBFramebuffer : public vrgb::IGBScreenDrawable
	{
	public:
		static const int	kFrameBufferWidth = 160;
		static const int	kFrameBufferHeight = 144;

	private:
		uint8			m_Framebuffer[kFrameBufferWidth * kFrameBufferHeight * 4];
		bool			m_RefreshScreen;

	public:
		CGBFramebuffer();

		virtual void	OnPreDraw() override;
		virtual void	OnPostDraw() override;
		virtual void	OnDrawPixel(int idColor, int x, int y) override;
		virtual void	OnRefreshScreen() override;
		virtual void	OnClear() override;

		bool	UniqueNeedRefresh();

		uint8*	Data() { return m_Framebuffer; }
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMeshComponents) TSubobjectPtr<UStaticMeshComponent> screen;
	UPROPERTY(EditDefaultsOnly, Category = Materials) UMaterialInterface *MasterMaterialRef;
	UPROPERTY() UTexture2D *DynamicTexture;
	UPROPERTY() UMaterialInstanceDynamic* DynamicMaterial;

	vrgb::VRGBEmulator	*m_Emulator;
	vrgb::Cartridge		*m_Cartridge;
	CGBFramebuffer		m_Framebuffer;

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
