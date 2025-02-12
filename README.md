# RebuildingProject
## [8번 과제] 게임 루프 및 UI 재설계하기
---
## 필수 과제 (기본 요구 사항)

### **필수 과제 1번 - 멀티 웨이브 구조 구현**

- 현재 프로젝트는 3번의 레벨이 전환이 됩니다.
- 이번에는 한 레벨 안에서 최소 **3단계 이상의 웨이브**를 만들어봅니다. (Wave 1, Wave 2, Wave 3)
- 각 웨이브는 일정 시간을 가지고, 웨이브가 증가할수록 스폰되는 아이템의 개수가 달라집니다.
- 시작 시점에 `UE_LOG`나 `GEngine->AddOnScreenDebugMessage`로 “Wave 1 시작!” 등 알림을 출력합니다.  

### **필수 과제 2번 - HUD & 메뉴 UI 리뉴얼**

- **HUD에 표시할 정보**: 점수, 시간, 체력을 전부 한 화면에서 볼 수 있도록 배치합니다.
- **메인 메뉴** (시작, 종료), **게임 오버 메뉴** (재시작, 메인 메뉴로 돌아가기) 화면을 재설계합니다.
- 폰트, 색상, 배경 등을 적절히 조정하여 **일관된 디자인**을 적용합니다.
- 개인이 원하는 스타일로 최대한 멋지고 직관적으로 UI를 디자인해봅니다.
- 각 버튼 클릭 시, C++ 혹은 블루프린트로 연결된 함수를 호출해 **실제 레벨 이동** 또는 **게임 종료**가 이루어지도록 구현합니다.

---

## 도전 과제 (선택 요구 사항)

### **도전 과제 1번 - 아이템 상호작용 로직 고도화**

- **새로운 부정적 아이템 효과**를 최소 두 가지 추가해 보세요. (단순 점수 감소 외에 색다른 디버프)
    - **SlowingItem**: 플레이어가 일정 시간 동안 이동 속도 50% 감소
    - **ReverseControlItem**: 일정 시간 동안 W키가 뒤로, A키가 오른쪽 등 컨트롤 반전
    - **BlindItem**: 일정 시간 화면 시야가 제한되거나, 카메라 회전이 제한
- 아이템 효과는 중첩될 수 있습니다.
- 효과의 시작/끝, 중첩 상황 등을 UI를 통해 표시하여 플레이어가 언제 디버프가 풀리는지 인지할 수 있도록 해주세요.

### **도전 과제 2번 - 웨이브별 환경의 변화**

- 웨이브가 올라갈수록 **맵 환경이나 플레이 상황 자체가 달라지게** 설계해봅시다. 각 웨이브마다 ‘새로운 장애물’이나 ‘무작위 이벤트’, ‘맵 변형’ 등을 도입해 난이도를 높일 수 있습니다. 예를 들어,
    - **Wave 1: 기존 로직 그대로**
    - **Wave 2:** 맵에 **새로운 장애물**이 추가됩니다. (예: 바닥에서 일정 간격으로 스파이크가 솟아오르는 지역)
    - **Wave 3:** 맵에 **무작위 폭발 지점**이 추가되어, 일정 시간마다 플레이어 주변에 충돌 이벤트가 발생합니다.
- “스파이크 함정이 활성화되었습니다!” 등 이벤트 발생을 UI로 알려서 플레이어가 상황을 인지할 수 있어야 합니다.

### **도전 과제 3번 - 고급 UI 연출하기 (애니메이션 & 3D 위젯 활용)**

- **UI 애니메이션 1개 이상 구현**
    - **Widget Animation**을 이용해 HUD나 메뉴의 **Fade In/Out**, **슬라이드** 애니메이션 등을 적용합니다.
    - 버튼 hover 상태에 **Tween** 또는 **강조** 효과를 주어, 사용자 클릭 욕구를 높이는 연출을 넣어봅니다.
- **3D 위젯 1개 이상 구현**
    - 캐릭터 머리 위 체력바, 아이템 상호작용 안내 문구 등을 3D 공간에 표시합니다.
    - `World Space` 모드나 `Screen Space` 모드 중 선택, 적절한 마스크·배경 처리를 합니다.
    - 마우스로 **3D 위젯을 직접 클릭**하여 상호작용할 수 있게 만들어봅시다.
        - 상호작용 아이템 “Press E to pick up” 등

---
---

## **구현 해야할 내용**

### **멀티 웨이브 구조**

- 한 레벨 안에서 **3단계 이상의 웨이브**를 구현 해야하므로 레벨 관련 로직을 웨이브 로직으로 변환 해준다.
- 웨이브 별로 `대기시간`을 주기위해 웨이브 시작전 준비하는 단계인 `PrepareWave`라는 이름의 함수를 만들어준다.

  <details>
  <summary>PrepareWave</summary>
  
  ```cpp
  
  void ASpartaGameState::PrepareWave()
  {
  	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
  	{
  		SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
  		if (SpartaPlayerController)
  		{
  			SpartaPlayerController->ShowGameHUD();
  			PlayerCharacter = Cast<ASpartaCharacter>(SpartaPlayerController->GetPawn());
  		}
  	}
  
  
  	if (UGameInstance* GameInstance = GetGameInstance())
  	{
  		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
  		{
  			CurrentWaveIndex = SpartaGameInstance->CurrentWaveIndex;
  		}
  	}
  
  	// 다음 웨이브 시작 딜레이
  	GetWorld()->GetTimerManager().SetTimer(
  		NextWaveStartTimerHadle,
  		this,
  		&ASpartaGameState::StartWave,
  		NextWaveStartDelay,
  		false
  	);
  }
  
  ```
    
  </details>


- 기존 `DataTable`도 각 웨이브 단계별로 아이템이 생성되게 사용하면 되므로 이름만 바꿔주고 구조는 그대로 사용한다.
- `ASpawnVolume`에 만들어 놨던 `UDataTable* ItemDataTable`을 `TArray<FWaveData> ItemDataTables`로 바꾸어 한 레벨에 적용할 웨이브를 여러개 적용 할 수 있도록 한다.
  > `FWaveData`는 웨이브 데이터 테이블(`UDataTable* ItemDataTable`)과 스폰 액터 개수(`int SpawnCount`)를 가진 구조체.

  <details>
    <summary>FWaveData</summary>

    ```cpp

    USTRUCT(BlueprintType)
    struct FWaveData
    {
    	GENERATED_BODY()
    
    public:
    	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    	UDataTable* ItemDataTable;
    	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    	int SpawnCount;
    };
    
    ```

  </details>

---

### **UI 리뉴얼**

- **메인 메뉴**
  - 게임 시작 버튼 : `MainLevel`로 이동하여 게임을 시작한다.
  - 게임 종료 버튼 : 게임을 종료한다.
  
<details>
  <summary>메인 메뉴</summary>

  ![image](https://github.com/user-attachments/assets/5dc7040d-8ab8-4928-a1d1-a21a8cd99add)

</details>

- **HUD**
  - 디버프 UI : 현재 플레이어가 걸린 `디버프`를 시각적으로 확인할 수 있다.
  - 시간 UI : `다음 웨이브 시작 대기 시간` 및 `현재 웨이브 남은 시간`을 확인할 수 있다.
  - 웨이브 및 점수 UI : `현재 웨이브 번호` 및 `현재 점수`를 확인할 수 있다.

    <details>
      <summary>전체 HUD</summary>
    
      ![image](https://github.com/user-attachments/assets/0ead6007-93ad-4958-9c2a-75bc4e3f42a6)
    
    </details>

---

### **아이템 상호작용 로직 고도화**
  - `ABaseItem`를 상속받는 새 클래스 `AStatusEffectItem`를 만들고 이를 상속받는 블루프린트 액터를 만들어 플레이어에게 효과를 부여하는 아이템들 만들었다.

    <details>
      <summary>AStatusEffectItem</summary>
    
      ```cpp
  
      UCLASS()
      class CH3_1_API AStatusEffectItem : public ABaseItem
      {
      	GENERATED_BODY()
      
      public:
      	AStatusEffectItem();
      
      	virtual void ActivateItem(AActor* Activator) override;
      
      	// 효과 타입
      	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|StatusEffectItem")
      	EStatusEffectType StatusEffectType;
      	// 효과 지속 시간
      	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|StatusEffectItem", meta = (ClampMin = "0.0", ClampMax = "10.0"))
      	float Duration;
      };
      
      ```
    
    </details>
    
  - `디버프 UI`를 만들어 시각적으로 현재 플레이어가 걸린 `디버프`와 남은 지속 시간를 시각적으로 확인할 수 있다.
  - 추가한 디버프
    - 슬로우 : 일정시간 동안 플레이어의 이동속도가 50% 감소한다.
    - 반전 : 일정시간 동안 플레이어의 입력 방향이 반대가 된다.
      > `W`입력시 뒤로가기, `D`입력시 왼쪽으로 가기.
    - 블라인드 : 일정시간 동안 플레이어의 시야가 어두워지며 흐려진다.

    <details>
      <summary>블라인드 효과</summary>
    
      ![image](https://github.com/user-attachments/assets/28feac88-ae29-4be2-8161-1f4d118e72df)
    
    </details>

    <details>
      <summary>효과 적용 로직</summary>
    
      ```cpp
  
      void ASpartaGameState::ActivateCharacterStatus(EStatusEffectType Type, float Duration)
      {
      	if (!PlayerCharacter) return;
      
      	FTimerHandle& CharacterStatusTimerHandle = CharacterStatus[Type];
      
      	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
      
      	FTimerDelegate TimerDel;
      
      	// 이전 지속시간 초기화
      	TimerManager.ClearTimer(CharacterStatusTimerHandle);
      
      	// 새로 적용
      	switch (Type)
      	{
      	case EStatusEffectType::SLOW:
      	{
      		PlayerCharacter->ActivateSlowing(true);
      		TimerDel.BindUFunction(PlayerCharacter, FName("ActivateSlowing"), false);
      	}
      	break;
      	case EStatusEffectType::REVERSECONTROL:
      	{
      		PlayerCharacter->ActivateReverseControl(true);
      		TimerDel.BindUFunction(PlayerCharacter, FName("ActivateReverseControl"), false);
      	}
      	break;
      	case EStatusEffectType::BLIND:
      	{
      		if (SpartaPlayerController)
      		{
      			if (UUserWidget* BlindEffectWidget = SpartaPlayerController->GetBlindEffectWidget())
      			{
      				UFunction* ActivateAnimFunc = BlindEffectWidget->FindFunction(FName("ActivateBlindEffect"));
      				
      				if (ActivateAnimFunc)
      				{
      					BlindEffectWidget->ProcessEvent(ActivateAnimFunc, nullptr);
      
      					TimerDel.BindLambda(
      						[BlindEffectWidget]()
      						{
      							UFunction* DeactivateAnimFunc = BlindEffectWidget->FindFunction(FName("DeactivateBlindEffect"));
      							BlindEffectWidget->ProcessEvent(DeactivateAnimFunc, nullptr);
      						});
      				}
      			}
      		}
      	}
      	break;
      	case EStatusEffectType::NONE:
      		// falls through(통과)
      	default:
      		break;
      	}
      
      	// 바운드 여부 확인
      	if (TimerDel.IsBound())
      	{
      		// 종료 타이머 실행
      		TimerManager.SetTimer(
      			CharacterStatusTimerHandle,
      			TimerDel,
      			Duration,
      			false
      		);
      	}
      }
      
      void ASpartaGameState::PrepareWave()
      {
      	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
      	{
      		SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
      		if (SpartaPlayerController)
      		{
      			SpartaPlayerController->ShowGameHUD();
      			PlayerCharacter = Cast<ASpartaCharacter>(SpartaPlayerController->GetPawn());
      		}
      	}
      
      
      	if (UGameInstance* GameInstance = GetGameInstance())
      	{
      		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
      		{
      			CurrentWaveIndex = SpartaGameInstance->CurrentWaveIndex;
      		}
      	}
      
      	// 다음 웨이브 시작 딜레이
      	GetWorld()->GetTimerManager().SetTimer(
      		NextWaveStartTimerHadle,
      		this,
      		&ASpartaGameState::StartWave,
      		NextWaveStartDelay,
      		false
      	);
      }
      
      ```
    
    </details>

---

### 고급 UI 연출하기 (애니메이션 & 3D 위젯 활용)

- 기존 캐릭터 `HP 위젯`을 텍스트 형태에서 `ProgressBar`를 활용하여 개선했다.

  <details>
    <summary>HP 위젯</summary>
  
    ![image](https://github.com/user-attachments/assets/1ec7f625-07d2-4bad-88a6-622efd66a212)
  
  </details>

- 버튼에 마우스를 올릴시(`호버 이벤트`) 이미지를 바꿔주어 강조 효과를 적용했다.

  <details>
    <summary>버튼 호버</summary>

    https://github.com/user-attachments/assets/bd210004-7904-4e67-a487-19fb4d311a4e

  </details>

---

### **최종 결과물**

https://github.com/user-attachments/assets/93385aab-e562-47c7-9cb0-2cb8fcfe3a32
