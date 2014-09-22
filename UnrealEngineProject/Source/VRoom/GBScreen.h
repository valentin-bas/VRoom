

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
		void	SetPadState(int idx, bool value);

		bool			m_keystates[8];
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMeshComponents) TSubobjectPtr<UStaticMeshComponent> screen;
	UPROPERTY(EditDefaultsOnly, Category = Materials) UMaterialInterface *MasterMaterialRef;
	UPROPERTY() UTexture2D* DynamicTexture;
	UPROPERTY() UMaterialInstanceDynamic* DynamicMaterial;

	CGBFramebuffer	m_Framebuffer;
	tgb::Emulator*	m_Emulator;

protected:
	virtual void Tick(float dt) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

	static AGBScreen* TargetGB;
	static AGBScreen* MasterGB;

	static unsigned char CallbackExtPort(unsigned char data);
	static bool GetLedStatut();

public:
	unsigned char SeriSend(unsigned char data);

	void UpButtonChangeState(bool pressed);
	void DownButtonChangeState(bool pressed);
	void LeftButtonChangeState(bool pressed);
	void RightButtonChangeState(bool pressed);
	void AButtonChangeState(bool pressed);
	void BButtonChangeState(bool pressed);
	void SelectButtonChangeState(bool pressed);
	void StartButtonChangeState(bool pressed);

	void UpButtonPressed() { m_Framebuffer.m_keystates[5] = true; }
	void DownButtonPressed() { m_Framebuffer.m_keystates[4] = true; }
	void LeftButtonPressed() { m_Framebuffer.m_keystates[6] = true; }
	void RightButtonPressed() { m_Framebuffer.m_keystates[7] = true; }
	void AButtonPressed() { m_Framebuffer.m_keystates[0] = true; }
	void BButtonPressed() { m_Framebuffer.m_keystates[1] = true; }
	void SelectButtonPressed() { m_Framebuffer.m_keystates[2] = true; }
	void StartButtonPressed() { m_Framebuffer.m_keystates[3] = true; }

	void UpButtonReleased() { m_Framebuffer.m_keystates[5] = false; }
	void DownButtonReleased() { m_Framebuffer.m_keystates[4] = false; }
	void LeftButtonReleased() { m_Framebuffer.m_keystates[6] = false; }
	void RightButtonReleased() { m_Framebuffer.m_keystates[7] = false; }
	void AButtonReleased() { m_Framebuffer.m_keystates[0] = false; }
	void BButtonReleased() { m_Framebuffer.m_keystates[1] = false; }
	void SelectButtonReleased() { m_Framebuffer.m_keystates[2] = false; }
	void StartButtonReleased() { m_Framebuffer.m_keystates[3] = false; }
};
