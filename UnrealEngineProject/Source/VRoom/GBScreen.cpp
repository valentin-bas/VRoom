

#include "VRoom.h"
#include "GBScreen.h"

#include <Emulator.h>


AGBScreen::AGBScreen(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
	, m_Emulator(nullptr)
{
	screen = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("screen"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("StaticMesh'/Game/MyAssets/Models/GBScreen_SM.GBScreen_SM'"));

	screen->SetStaticMesh(StaticMesh.Object);
	RootComponent = screen;

	PrimaryActorTick.bCanEverTick = true;
}

void	AGBScreen::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//create dynamic material
	DynamicMaterial = UMaterialInstanceDynamic::Create(MasterMaterialRef, RootComponent);

	// create dynamic texture
	DynamicTexture = UTexture2D::CreateTransient(CGBFramebuffer::kFrameBufferWidth, CGBFramebuffer::kFrameBufferHeight);
	DynamicTexture->AddToRoot();
	DynamicTexture->UpdateResource();

	screen->SetMaterial(0, DynamicMaterial);
	DynamicMaterial->SetTextureParameterValue(FName("T2DTexture"), DynamicTexture);

	m_Emulator = new tgb::Emulator(m_Framebuffer);				//CHECK //LEAK
	if (!m_Emulator->LoadCartridge("tetris.gb"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to load rom"));
	}
	UE_LOG(LogTemp, Warning, TEXT("Rom loaded"));
}

void	AGBScreen::BeginPlay()
{
	Super::BeginPlay();

	FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
	if (Iterator)
	{
		EnableInput(*Iterator);
	}
}

void	AGBScreen::Tick(float dt)
{
	Super::Tick(dt);

	m_Emulator->Update(); //FIXME // DO IT IN A THREAD ?

	static FUpdateTextureRegion2D Region = FUpdateTextureRegion2D(0, 0, 0, 0, CGBFramebuffer::kFrameBufferWidth, CGBFramebuffer::kFrameBufferHeight);
	if (m_Framebuffer.UniqueNeedRefresh())
	{
		DynamicTexture->UpdateResource();
		UpdateTextureRegions(DynamicTexture, 0, 1, &Region, CGBFramebuffer::kFrameBufferWidth * 4, 4, m_Framebuffer.Data(), false);
	}
}

void	AGBScreen::UpButtonChangeState(bool pressed)
{
	//vrgb::Pad::SetState(vrgb::Pad::gbUP, pressed);
}

void	AGBScreen::DownButtonChangeState(bool pressed)
{
	//vrgb::Pad::SetState(vrgb::Pad::gbDOWN, pressed);
}

void	AGBScreen::LeftButtonChangeState(bool pressed)
{
	//vrgb::Pad::SetState(vrgb::Pad::gbLEFT, pressed);
}

void	AGBScreen::RightButtonChangeState(bool pressed)
{
	//vrgb::Pad::SetState(vrgb::Pad::gbRIGHT, pressed);
}

void	AGBScreen::AButtonChangeState(bool pressed)
{
	//vrgb::Pad::SetState(vrgb::Pad::gbA, pressed);
}

void	AGBScreen::BButtonChangeState(bool pressed)
{
	//vrgb::Pad::SetState(vrgb::Pad::gbB, pressed);
}

void	AGBScreen::SelectButtonChangeState(bool pressed)
{
	//vrgb::Pad::SetState(vrgb::Pad::gbSELECT, pressed);
}

void	AGBScreen::StartButtonChangeState(bool pressed)
{
	//vrgb::Pad::SetState(vrgb::Pad::gbSTART, pressed);
}


void	AGBScreen::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture && Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			UpdateTextureRegionsData,
			FUpdateTextureRegionsData*, RegionData, RegionData,
			bool, bFreeData, bFreeData,
			{
			for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
			{
				int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
				if (RegionData->MipIndex >= CurrentFirstMip)
				{
					RHIUpdateTexture2D(
						RegionData->Texture2DResource->GetTexture2DRHI(),
						RegionData->MipIndex - CurrentFirstMip,
						RegionData->Regions[RegionIndex],
						RegionData->SrcPitch,
						RegionData->SrcData
						+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
						+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
						);
				}
			}
			if (bFreeData)
			{
				FMemory::Free(RegionData->Regions);
				FMemory::Free(RegionData->SrcData);
			}
			delete RegionData;
			});
	}
}

AGBScreen::CGBFramebuffer::CGBFramebuffer()
	: m_RefreshScreen(false)
{
}

bool	AGBScreen::CGBFramebuffer::UniqueNeedRefresh()
{
	if (m_RefreshScreen)
	{
		m_RefreshScreen = false;
		return true;
	}
	return false;
}

void	AGBScreen::CGBFramebuffer::refresh()
{
	m_RefreshScreen = true;
	m_padstate = 0;
}

void	AGBScreen::CGBFramebuffer::render_screen(byte *buf, int width, int height, int depth)
{
	uint16* wbuf = (uint16*)buf;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			uint16	gb_col = *wbuf++;
			uint8	r = ((gb_col >> 0) & 0x1f) << 3;
			uint8	g = ((gb_col >> 5) & 0x1f) << 3;
			uint8	b = ((gb_col >> 10) & 0x1f) << 3;

			m_Framebuffer[y * kFrameBufferWidth * 4 + x * 4 + 0] = r;
			m_Framebuffer[y * kFrameBufferWidth * 4 + x * 4 + 1] = g;
			m_Framebuffer[y * kFrameBufferWidth * 4 + x * 4 + 2] = b;
			m_Framebuffer[y * kFrameBufferWidth * 4 + x * 4 + 3] = 255;
		}
	}
}

int		AGBScreen::CGBFramebuffer::check_pad()
{
	return m_padstate;
}