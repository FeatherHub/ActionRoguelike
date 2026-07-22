# Action Roguelike

> Unreal Engine 5.6과 C++로 제작한 3인칭 멀티플레이어 슈팅 로그라이크 프로토타입입니다. 

## 프로젝트 개요

| 항목 | 내용 |
| --- | --- |
| 장르 | 3인칭 액션 로그라이크 |
| 개발 형태 | 개인 프로젝트 |
| 엔진 | Unreal Engine 5.6.1 |
| 주요 언어 | C++, Blueprint |
| 핵심 기술 | Gameplay Tags, Enhanced Input, AI Behavior Tree, EQS, UMG, Niagara, SaveGame, RPC/Replication, World Subsystem, ISM, Unreal Insights Trace, Developer Settings |

## 핵심 플레이 요소들
- 일반 마법 탄환, 블랙홀, 순간이동 투사체
- 체력·분노·이동 속도 캐릭터 어트리뷰트
- 화상과 기절을 포함한 지속 시간 / 주기 / 중첩형 상태 효과들
- 사거리/시야 판정, 사격권 내로 이동, 체력 기반 도피 및 자가 회복을 수행하는 적 AI
- 거리와 시선 방향을 가중 합산하는 월드 오브젝트 상호작용 선택
- 체력 포션, 코인 수집, 크레딧 누적
- 월드 상태와 플레이어 크레딧을 보존하는 세이브/로드
- 월드 공간 체력바, 상호작용 프롬프트, 상태 효과 아이콘, 인게임 메뉴

입력 키는 Enhanced Input의 `IMC_DefaultPlayer`에서 변경할 수 있으며,  
플레이어가 사용하는 논리 입력은 Move, Look, Jump, Sprint, Primary Attack, Blackhole Attack, Teleport, Interact, In-game Menu가 있습니다.

### 디렉터리 구성

```text
Source/ActionRoguelike/
├─ ActionSystem/   # 액션, 상태 효과, 어트리뷰트, 태그 기반 실행 규칙
├─ AI/             # AI 캐릭터/컨트롤러, BT Task·Service, EQS Context
├─ Animation/      # Gameplay Tag와 애니메이션 상태 연결
├─ Core/           # GameMode, PlayerState, 게임 태그, 충돌 채널, 설정
├─ Development/    # 네트워크 디버그 컨텍스트와 화면 출력 도구
├─ Pickup/         # 포션과 ISM 기반 코인 월드 서브시스템
├─ Player/         # 캐릭터, 컨트롤러, 상호작용 컴포넌트
├─ Projectile/     # 공통 투사체와 공격별 파생 구현
├─ SaveSystem/     # 범용 Actor 직렬화 컴포넌트와 SaveGame 데이터
├─ Widget/         # 월드 좌표를 화면 UI에 동기화하는 위젯 기반 클래스
└─ World/          # 상자와 폭발 배럴 등 상호작용 오브젝트
```

## 주요 구현 내용

### 1. Gameplay Tag 기반 커스텀 액션 시스템

`URogueActionSystemComponent`가 캐릭터의 액션, 어트리뷰트, 활성 태그를 한곳에서 관리합니다. 각 액션은 `URogueActionBase`를 상속하는 독립 `UObject`이므로 캐릭터 상속 계층을 키우지 않고 Blueprint 클래스로 기능과 데이터를 확장할 수 있습니다.

- `ActivationBlockedTags`로 실행 불가 상태 판정
- `ActivationGrantTags`로 실행 중 상태 부여
- 어트리뷰트별 비용과 쿨다운 검사
- 액션 클래스 런타임 Grant/Remove 지원
- `ReplicateSubobjects()`를 통한 액션·어트리뷰트 `UObject` 복제

예를 들어 Sprint 액션은 실행 중 `Status.Sprinting` 태그를 부여하고 이동 속도 배율을 변경합니다.  
기절 효과가 해당 액션의 차단 태그와 일치하면 액션 시스템이 Sprint를 중단하며, 애니메이션 인스턴스도 같은 태그 이벤트를 받아 상태를 갱신합니다.

### 2. 데이터 중심 어트리뷰트와 이벤트 전달

어트리뷰트는 아래 계산식을 공유하는 `FRogueAttribute`로 표현했습니다.

```text
Final Value = (Base + Modifier) × Multiplier
```

플레이어와 몬스터는 서로 다른 AttributeSet을 사용합니다. 
컴포넌트 초기화 시 리플렉션으로 `FRogueAttribute` 프로퍼티를 탐색하고 `Attribute.<PropertyName>` 태그에 자동 매핑하므로, 호출자는 태그만으로 값을 조회하거나 변경할 수 있습니다.

Attribute의 값이 변하면 C++ 멀티캐스트 델리게이트와 Blueprint 동적 델리게이트로 전달됩니다.

### 3. 지속·주기·중첩형 Action Effect

`URogueActionEffect`는 기본 액션 위에 다음 수명 주기를 추가합니다.

- Effect Duration 기반 자동 만료
- Period Interval 기반 반복 실행
- 최초 적용 시 즉시 실행 옵션
- 동일 효과 재적용 시 Stack 증가 및 만료 시간 갱신
- 서버 시간 기준 남은 시간 계산
- 추가/제거 델리게이트를 이용한 위젯 아이콘 동기화
- 잘못된 Duration/Period 조합을 찾는 Editor Data Validation

공격 투사체는 구체적인 효과 구현을 알지 않고 `TSubclassOf<URogueActionEffect>`만 전달합니다. 따라서 Burning, Stun 외의 효과도 투사체 코드를 수정하지 않고 추가할 수 있습니다.

### 4. 서버 실행과 복제 경로

액션 입력은 로컬에서 즉시 반응하면서 Reliable Server RPC로 서버 실행을 요청합니다. 액션 실행 상태와 액션/어트리뷰트 서브오브젝트 자체도 복제합니다.

- `ServerStartAction` / `ServerStopAction`
- 서버 RPC 기반 상호작용 실행

### 5. EQS와 Behavior Tree 기반 AI

GameMode는 일정 간격으로 생존 AI 수를 계산하고, 시간에 따른 Curve 값으로 최대 개체 수를 결정합니다. 추가 스폰이 필요하면 EQS를 비동기로 실행해 상위 후보군에서 위치를 선택합니다. 진행 중인 Query Instance를 보관해 중복 실행을 막고, 성공·실패 콜백에서 상태를 초기화합니다.

AI 전투는 다음 노드로 나누었습니다.

- 사거리와 Line of Sight를 함께 평가하는 BT Service
- 현재/최대 체력 비율로 회복 분기를 여는 BT Service
- 탄 퍼짐을 적용해 투사체를 생성하는 원거리 공격 Task
- 공통 어트리뷰트 API를 사용하는 자가 회복 Task
- Blackboard의 대상 Actor를 EQS Context로 공급하는 커스텀 Context
- 사망 이벤트 수신 후 Brain Logic 중단

### 6. 대량 코인을 위한 World Subsystem

코인마다 Actor를 생성하지 않고 `UTickableWorldSubsystem` 하나가 위치, 보상, 인스턴스 ID를 병렬 배열로 관리합니다. 렌더링은 `UInstancedStaticMeshComponent`로 일괄 처리하며 플레이어와의 거리 검사 결과를 모아 한 번에 크레딧으로 지급합니다.

- 코인 Mesh와 Sound를 Developer Settings의 Soft Reference로 비동기 로드
- Navigation System을 이용한 반경 내 랜덤 배치
- ISM 인스턴스 ID 기반 개별 제거
- Unreal Insights용 CPU Scope와 전체 코인 수 Counter 계측
- 여러 코인을 연속 획득할 때 MetaSound 컴포넌트와 Trigger Parameter 활용한 효과음 재생

#### Unreal Insights 계측

최적화가 추측에 머무르지 않도록 코인 시스템 내부에 Unreal Trace 계측 지점을 직접 배치했습니다.

| Trace 항목 | 계측 대상 |
| --- | --- |
| `TotalCoinCount` | 현재 월드에 존재하는 전체 코인 수를 추적하는 Integer Counter |
| `URogueCoinPickupSubsystem::Tick::DistanceCheck` | 플레이어와 각 코인 사이의 거리 검사 비용 |
| `URogueCoinPickupSubsystem::Tick::PickupCoins` | 선택된 코인의 보상 합산과 제거 비용 |
| `URogueCoinPickupSubsystem::SpawnCoins::GetNavRandLocations` | Navigation System에서 랜덤 위치를 구하는 비용 |
| `URogueCoinPickupSubsystem::SpawnCoins::AddISMInstance` | Transform 생성과 ISM 인스턴스 추가 비용 |

`TRACE_COUNTER_SET`은 월드 시작 시 0으로 초기화되고, 코인 추가·제거가 끝날 때 실제 배열 크기로 다시 기록됩니다. 따라서 Unreal Insights에서 개체 수 변화와 각 CPU Scope의 비용을 같은 타임라인에서 비교할 수 있습니다.

#### Custom Project Settings

코인 시스템이 콘텐츠 경로를 C++에 하드코딩하지 않도록 `URoguePickupSystemSetting : UDeveloperSettings`를 구현했습니다.

- `UCLASS(Config=Game, DefaultConfig)`로 프로젝트 단위 설정 제공
- `GetCategoryName()`에서 프로젝트명을 반환해 **Project Settings > ActionRoguelike** 카테고리에 노출
- 코인 Mesh와 획득 Sound를 `TSoftObjectPtr`로 보관
- 오디오의 Trigger Parameter 이름도 설정 데이터로 분리
- 설정값은 `Config/DefaultGame.ini`에 저장
- `GetDefault<URoguePickupSystemSetting>()`으로 런타임 기본 설정 조회
- `LoadAsync()`와 `FLoadSoftObjectPathAsyncDelegate`로 Mesh/Sound 비동기 로드


### 7. 재사용 가능한 상호작용과 저장 시스템

상호작용 컴포넌트는 카메라 주변의 후보를 수집한 뒤 거리와 시선 방향에 가중치를 곱한 후 합산하여 가장 높은 값을 가진 대상을 선택합니다. 클라이언트는 대상 위에 인월드 상호작용 위젯을 표시하고 실제 `IRogueInteractionInterface::Interact` 호출은 서버 RPC를 통해 실행합니다.

저장 기능은 `URogueSaveComponent`를 부착한 Actor를 GameMode가 자동 수집하는 방식입니다. Actor의 Transform과 `SaveGame` 지정 프로퍼티를 `FObjectAndNameAsStringProxyArchive`로 바이트 배열에 직렬화하므로, 오브젝트마다 별도의 저장 구조체를 반복 작성할 필요가 없습니다.

### 8. 개발·디버깅 도구

네트워크 플레이를 관찰하기 위해 `URogueDebugSubsystem : UWorldSubsystem` 기반 화면 로그 큐와 전용 매크로를 구현했습니다. 호출 지점은 메시지를 즉시 화면에 그리지 않고 `FScreenDebugContext`로 제출하며, 서브시스템이 `FWorldDelegates::OnWorldTickEnd`에서 해당 월드의 큐를 일괄 출력하고 수명을 갱신합니다.

각 디버그 컨텍스트에는 다음 정보가 포함됩니다.

- `FDateTime` 기반 `MM:SS.mmm` 타임스탬프
- PIE Instance ID와 Standalone/Server/Client NetMode
- Actor의 Local Role과 Authority 여부
- PlayerController/Pawn의 Local Control 여부
- NetDriver GuidCache를 이용한 Network GUID 기반 오브젝트 이름
- 서버는 Green, 클라이언트는 Blue로 구분되는 기본 색상

`ROGUE_DEBUG`, `ROGUE_DEBUGFMT`, `ROGUE_DEBUG_CVAR`, `ROGUE_DEBUG_CVARFMT` 매크로를 제공하며, Object Hash·Subkey·서버 여부를 조합한 Debug Key를 사용합니다. 같은 호출 지점의 메시지는 화면을 계속 추가하는 대신 기존 줄을 갱신할 수 있습니다.

주요 CVar 예시는 다음과 같습니다.

| CVar | 용도 |
| --- | --- | ---: |
| `rogue.net.debug.ShowContext` | 메시지 앞에 PIE/NetMode/Role/Authority/Control 컨텍스트 표시 |
| `rogue.asc.attribute.ShowMsg` | 어트리뷰트 변경 화면 메시지 |
| `rogue.interaction.Debugdraw` | 탐색 반경, 후보 Bounds, 가중치 시각화 |
| `rogue.worldwidget.DebugDraw` | 월드 좌표와 화면 투영 상태 표시 |
| `rogue.projectile.DebugDraw` | 카메라 Trace, 보정 경로, 원래 조준 경로 표시 및 표시 시간 지정 |
| `rogue.ai.minionranged.DebugDraw` | AI 사거리 원과 공격 방향 표시 |
| `rogue.gamemode.spawnbot.ShowDebug` | 생존/최대 Bot 수와 EQS 스폰 결과 표시 |
| `rogue.gamemode.savesystem.ShowDebug` | 저장 성공 여부 표시 |

`rogue.net.debug.Filter`와 필터 판정 함수도 Client/Server/Both 모드로 정의되어 있습니다. 다만 현재 `SubmitDebugContext()` 경로에는 이 판정이 연결되어 있지 않아, 실제 필터 적용은 개선 과제로 분리했습니다.

### 9. 투사체와 전투 피드백

공통 `ARogueProjectileBase`는 복제, Sphere Collision, 무중력 Projectile Movement, 비행 중 Niagara/Audio Component, 생성·폭발 효과, 수명 관리를 담당합니다. 파생 클래스는 충돌 이후의 규칙만 정의합니다.

| 투사체 | 동작 |
| --- | --- |
| Magic | Point Damage 적용 후 폭발 |
| Blackhole | 음수 Radial Force로 주변 물체를 끌어당기며 Overlap 대상을 제거하고 설정 시간 후 폭발 |
| Teleport | 충돌하거나 타이머가 만료되면 Instigator를 투사체 위치로 이동하고 제거 |

플레이어 투사체 공격은 카메라 시점에서 최대 5,000 단위 Trace로 목표 지점을 구한 뒤, 캐릭터 Mesh의 Muzzle Socket에서 해당 지점을 향해 발사합니다. 카메라와 총구의 위치 차이로 생기는 조준 오차를 보정합니다.

전투 피드백도 게임플레이 로직과 연결했습니다.
- 피격 시 Overlay Material과 Custom Primitive Data에 게임 시간을 전달해 Hit Flash 표현
- 플레이어 체력이 0이 되면 입력 비활성화와 사망 Montage 재생
- AI 체력이 0이 되면 월드 체력바 제거, Death Delegate 전달, Brain Logic 중지, Montage 길이만큼 LifeSpan 설정
- 폭발 배럴은 Magic Projectile 충돌 후 Fuse Niagara/Audio를 재사용하고 지연 폭발
- 체력 포션은 실제 체력 값이 증가한 경우에만 효과음 재생 후 제거

### 10. 충돌, 입력, UI의 경계 분리

#### Custom Collision Channel/Profile

충돌 정책은 하드코딩된 문자열 사용을 줄이기 위해 `RogueCollision` 네임스페이스와 Project Collision Settings를 함께 사용합니다.

| 종류 | 설정 |
| --- | --- |
| `Interaction` Trace Channel | `ECC_GameTraceChannel1`, 기본 Ignore |
| `Projectile` Object Channel | `ECC_GameTraceChannel2`, 기본 Ignore |
| `Projectile` Profile | Query Only, Visibility/Camera Ignore |
| `Interaction` Profile | PhysicsBody 기반, Interaction Overlap, Projectile Block |
| `Pickup` Profile | Query Only, Pawn/Projectile Overlap, 나머지 주요 채널 Ignore |

#### World-space UI와 메뉴

`URogueWorldWidget`은 Actor 위치에 Offset을 더한 월드 좌표를 `ProjectWorldToScreen()`으로 투영합니다. 결과를 Viewport Scale로 나눠 DPI 스케일을 보정한 뒤, BindWidget으로 연결된 `USizeBox`의 Render Translation을 갱신합니다. 
대상 Actor가 사라지면 위젯도 Viewport에서 자동 제거됩니다.

### 11. C++와 Blueprint의 역할 분리

| C++ | Blueprint/Asset |
| --- | --- |
| 액션 실행 조건, 비용, 쿨다운, 태그 수명 주기 | Magic/Blackhole/Teleport/Sprint 액션별 데이터 |
| 어트리뷰트 계산, 델리게이트, 복제 | Burning/Stun 효과의 주기 실행과 아이콘 |
| 투사체 충돌·피해·이동 기반 클래스 | Mesh, Niagara, Sound, Montage 조립 |
| BT Task/Service와 EQS Context | `BT_MinionRanged`, `BB_MinionRanged`, EQS Query |
| 월드 좌표 투영과 위젯 수명 관리 | HUD, 체력바, 효과 아이콘, 상호작용 프롬프트 레이아웃 |
| 저장 직렬화와 Actor 검색 | 저장 호출 UI와 `OnSaveLoaded` 시각 반영 |

Gameplay Tag도 코드와 데이터 양쪽에서 관리합니다. 공격·기본 어트리뷰트·Sprint/Stun 상태처럼 코드 전반에서 참조되는 태그는 Native Gameplay Tag로 선언하고, `Action.Stun`, `Attribute.MoveSpeed`, `Status.Burning`처럼 콘텐츠에서 확장하는 태그는 `DefaultGameplayTags.ini`에 등록했습니다.

### 12. 에디터 데이터 검증 및 런타임 검증

- `URogueActionEffect::IsDataValid()`에서 Duration/Period 누락과 Period가 Duration보다 긴 설정을 경고
- `ARogueGameMode::IsDataValid()`에서 Bot Spawn Curve 누락을 Invalid로 보고
- `ensure`, `ensureMsgf`, `check`로 필수 런타임 참조와 불변 조건 검증

