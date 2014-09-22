

#include "VRoom.h"
#include "GBScreen.h"

#include <Emulator.h>

AGBScreen* AGBScreen::TargetGB = nullptr;
AGBScreen* AGBScreen::MasterGB = nullptr;


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
	m_Emulator->HookExport(AGBScreen::CallbackExtPort, AGBScreen::GetLedStatut);
}

void	AGBScreen::BeginPlay()
{
	Super::BeginPlay();

	FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
	if (Iterator)
	{
		EnableInput(*Iterator);
	}
	UE_LOG(LogTemp, Warning, TEXT("Name : %s"), *GetName());
	if (GetName() == "GBScreen_BP_C_1")
	{
		MasterGB = this;
		InputComponent = GetWorld()->GetFirstPlayerController()->InputComponent;
		InputComponent->BindAction("GBUpKey", IE_Pressed, this, &AGBScreen::UpButtonPressed);
		InputComponent->BindAction("GBDownKey", IE_Pressed, this, &AGBScreen::DownButtonPressed);
		InputComponent->BindAction("GBLeftKey", IE_Pressed, this, &AGBScreen::LeftButtonPressed);
		InputComponent->BindAction("GBRightKey", IE_Pressed, this, &AGBScreen::RightButtonPressed);
		InputComponent->BindAction("GBSelectKey", IE_Pressed, this, &AGBScreen::SelectButtonPressed);
		InputComponent->BindAction("GBStartKey", IE_Pressed, this, &AGBScreen::StartButtonPressed);
		InputComponent->BindAction("GBAKey", IE_Pressed, this, &AGBScreen::AButtonPressed);
		InputComponent->BindAction("GBBKey", IE_Pressed, this, &AGBScreen::BButtonPressed);

		InputComponent->BindAction("GBUpKey", IE_Released, this, &AGBScreen::UpButtonReleased);
		InputComponent->BindAction("GBDownKey", IE_Released, this, &AGBScreen::DownButtonReleased);
		InputComponent->BindAction("GBLeftKey", IE_Released, this, &AGBScreen::LeftButtonReleased);
		InputComponent->BindAction("GBRightKey", IE_Released, this, &AGBScreen::RightButtonReleased);
		InputComponent->BindAction("GBSelectKey", IE_Released, this, &AGBScreen::SelectButtonReleased);
		InputComponent->BindAction("GBStartKey", IE_Released, this, &AGBScreen::StartButtonReleased);
		InputComponent->BindAction("GBAKey", IE_Released, this, &AGBScreen::AButtonReleased);
		InputComponent->BindAction("GBBKey", IE_Released, this, &AGBScreen::BButtonReleased);

		for (TActorIterator<AGBScreen> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr->GetName() == "GBScreen_BP_2")
				TargetGB = *ActorItr;
		}
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

unsigned char	AGBScreen::SeriSend(unsigned char data)
{
	return m_Emulator->SeriSend(data);
}

unsigned char	AGBScreen::CallbackExtPort(unsigned char data)
{
	unsigned char ret = 0;

	if (TargetGB)
	{
		UE_LOG(LogTemp, Warning, TEXT("Send : %c"), data);
		ret = TargetGB->SeriSend(data);
		UE_LOG(LogTemp, Warning, TEXT("Receive : %c"), ret);
	}
	return ret;
}

bool	AGBScreen::GetLedStatut()
{
	return true;
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
	, m_padstate(0)
{
	for (int i = 0; i < 8; ++i)
		m_keystates[i] = false;
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
	for (int i = 0; i < 8; ++i)
		m_padstate |= m_keystates[i] ? (1 << i) : 0;
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

void	AGBScreen::CGBFramebuffer::SetPadState(int idx, bool value)
{
	m_keystates[idx] = value;;
}