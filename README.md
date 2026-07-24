## 프로젝트 개요

| 항목 | 내용 |
| --- | --- |
| 장르 | 3인칭 슈팅 액션 로그라이크 |
| 개발 | 개인 프로젝트 |
| 엔진 | Unreal Engine 5.6.1 |
| 사용 기술 | Gameplay Tags, Enhanced Input, Behavior Tree, EQS, UMG, Niagara, SaveGame, RPC/Replication, World Subsystem, ISM, Unreal Insights Trace, Developer Settings |

## 핵심 플레이 요소들
- 일반 마법 탄환, 블랙홀, 순간이동 투사체
- 체력·분노·이동 속도 캐릭터 어트리뷰트
- 화상과 기절 상태효과 
- 캐릭터 근처를 배회, 사거리/시야 판정으로 사격권 내로 이동 후 공격, 체력 기반 도피 및 자가 회복을 수행하는 적 AI
- 거리와 시선 방향을 가중 합산하는 월드 오브젝트 상호작용 선택
- 체력 포션, 코인 수집, 크레딧 수집
- 월드 상태와 플레이어 크레딧을 보존하는 세이브/로드 시스템
- 인월드 체력비, 상호작용 프롬프트 위젯
- 상태 효과 아이콘, 인게임 메뉴
  
## 디렉터리 구성

| 경로 | 내용 |
| --- | --- |
| [`ActionSystem`](/Source/ActionRoguelike/ActionSystem) | 액션, 상태 효과, 어트리뷰트, Gameplay Tag 규칙 |
| [`AI`](/Source/ActionRoguelike/AI) | AI 캐릭터·컨트롤러, BT Task·Service, EQS Context |
| [`Animation`](/Source/ActionRoguelike/Animation) | Gameplay Tag와 애니메이션 상태 연결 |
| [`Core`](/Source/ActionRoguelike/Core) | GameMode, PlayerState, 태그, 충돌 채널, 프로젝트 설정 |
| [`Development`](/Source/ActionRoguelike/Development) | 네트워크 컨텍스트와 화면 디버그 도구 |
| [`Pickup`](/Source/ActionRoguelike/Pickup) | 포션과 ISM 기반 코인 서브시스템 |
| [`Player`](/Source/ActionRoguelike/Player) | 플레이어 캐릭터·컨트롤러와 상호작용 |
| [`Projectile`](/Source/ActionRoguelike/Projectile) | 공통 투사체와 공격별 구현 |
| [`SaveSystem`](/Source/ActionRoguelike/SaveSystem) | Actor 직렬화와 SaveGame 데이터 |
| [`Widget`](/Source/ActionRoguelike/Widget) | 월드 좌표를 화면 UI에 동기화하는 위젯 |
| [`World`](/Source/ActionRoguelike/World) | 상자와 폭발 배럴 등 월드 오브젝트 |

# 구현 내용

## 커스텀 액션 시스템

`URogueActionSystemComponent`가 캐릭터의 액션, 어트리뷰트, 활성 태그를 관리합니다. 각 액션은 `URogueActionBase`를 상속하는 독립 `UObject`이며, 실행 중 여부, 쿨다운, 차단 태그와 비용을 검사한 뒤 시작합니다.

- `ActivationBlockedTags`로 실행 불가 상태 판정
- `ActivationGrantTags`로 실행 중 상태 부여
- 어트리뷰트별 비용과 쿨다운 검사
- 액션 클래스 런타임 Grant/Remove 지원
- `ReplicateSubobjects()`를 통한 액션·어트리뷰트 `UObject` 복제

예를 들어 Sprint 액션은 실행 중 `Status.Sprinting` 태그를 부여하고 이동 속도 배율을 변경합니다.  
기절 효과가 해당 액션의 차단 태그와 일치하면 액션 시스템이 Sprint를 중단하며, 애니메이션 인스턴스도 같은 태그 이벤트를 받아 상태를 갱신합니다.

- [`RogueActionBase.cpp`](/Source/ActionRoguelike/ActionSystem/RogueActionBase.cpp): Blueprint 확장 지점과 액션 데이터, 태그 부여·해제와 실행 조건
- [`RogueActionSystemComponent.cpp`](/Source/ActionRoguelike/ActionSystem/RogueActionSystemComponent.cpp): 액션 Grant·Remove와 실행

---

## Attribute

`FRogueAttribute`는 `(Base + Modifier) × Multiplier`로 값을 계산합니다. AttributeSet의 프로퍼티는 리플렉션으로 탐색해 `Attribute.<PropertyName>` 태그와 연결하고, 값 변화는 C++·Blueprint 델리게이트로 전달합니다.

- [`RogueAttributeSet.h`](/Source/ActionRoguelike/ActionSystem/RogueAttributeSet.h): 어트리뷰트 계산식과 캐릭터별 AttributeSet
- [`RogueActionSystemComponent.cpp`](/Source/ActionRoguelike/ActionSystem/RogueActionSystemComponent.cpp): 리플렉션 기반 태그 매핑, 값 변경과 델리게이트 전달

---

## Action Effect

`URogueActionEffect`는 지속 시간, 주기 실행, 최초 즉시 실행과 동일 효과 재적용 시 중첩·만료 시간 갱신을 담당합니다.

- [`RogueActionEffect.cpp`](/Source/ActionRoguelike/ActionSystem/RogueActionEffect.cpp): 효과 실행, 만료, 중첩과 남은 시간
- [`RogueGameplayStatics.cpp`](/Source/ActionRoguelike/ActionSystem/RogueGameplayStatics.cpp): 투사체 피해와 효과 적용 연결

---

## 서버 실행과 복제 경로

액션 입력은 로컬에서 즉시 반응하면서 Reliable Server RPC로 서버 실행을 요청합니다.

- [`RogueActionSystemComponent.h`](/Source/ActionRoguelike/ActionSystem/RogueActionSystemComponent.h): RPC와 액션·어트리뷰트 서브오브젝트 복제
- [`RogueAction_ProjectileAttack.cpp`](/Source/ActionRoguelike/ActionSystem/RogueAction_ProjectileAttack.cpp): 서버 투사체 생성과 조준 보정

---

## EQS와 Behavior Tree 기반 AI

- [`RogueBTService_WithinRange.cpp`](/Source/ActionRoguelike/AI/RogueBTService_WithinRange.cpp): 사거리와 Line of Sight 판정
- [`RogueBTService_CheckHealthLow.cpp`](/Source/ActionRoguelike/AI/RogueBTService_CheckHealthLow.cpp): 체력 비율 판정
- [`RogueBTTask_RangedAttack.cpp`](/Source/ActionRoguelike/AI/RogueBTTask_RangedAttack.cpp): 탄 퍼짐을 적용한 원거리 공격
- [`RogueBTTask_HealSelf.cpp`](/Source/ActionRoguelike/AI/RogueBTTask_HealSelf.cpp): 공통 어트리뷰트 API를 사용한 회복

---

## 대량 코인을 위한 World Subsystem

`URogueCoinPickupSubsystem`은 코인을 Actor로 생성하지 않고 위치·보상·ISM 인스턴스 ID로 관리합니다. Navigation System으로 배치하고, 거리 검사 후 인스턴스를 제거하며 크레딧을 지급합니다.

- [`RogueCoinPickupSubsystem.h`](/Source/ActionRoguelike/Pickup/RogueCoinPickupSubsystem.h): 거리 검사와 획득 처리, Navigation 배치와 ISM 추가·제거

---

## Unreal Insights

코인 시스템의 거리 검사, 획득, 위치 탐색과 ISM 추가 비용을 CPU Scope로 구분하고, 현재 코인 수를 Counter로 기록합니다.

| Trace 항목 | 계측 대상 |
| --- | --- |
| `TotalCoinCount` | 현재 코인 수 |
| `Tick::DistanceCheck` | 플레이어와 코인의 거리 검사 |
| `Tick::PickupCoins` | 보상 합산과 코인 제거 |
| `SpawnCoins::GetNavRandLocations` | Navigation 위치 탐색 |
| `SpawnCoins::AddISMInstance` | Transform 생성과 ISM 추가 |

- [`RogueCoinPickupSubsystem.cpp`](/Source/ActionRoguelike/Pickup/RogueCoinPickupSubsystem.cpp)
  
---

## Custom Project Settings

코인 시스템이 콘텐츠 경로를 에디터 프로젝트 세팅에서 설정할 수 있도록 `URoguePickupSystemSetting : UDeveloperSettings`를 구현했습니다.

- [`RoguePickupSystemSetting.h`](/Source/ActionRoguelike/Core/RoguePickupSystemSetting.h): Soft Reference와 프로젝트 설정 카테고리
  
---

## 재사용 가능한 상호작용 시스템과 저장 시스템

상호작용 컴포넌트는 카메라 주변의 후보를 수집한 뒤 거리와 시선 방향에 가중치를 곱한 후 합산하여 가장 높은 값을 가진 대상을 선택합니다. 클라이언트는 대상 위에 인월드 상호작용 위젯을 표시하고 실제 `IRogueInteractionInterface::Interact` 호출은 서버 RPC를 통해 실행합니다.

- [`RogueInteractionComponent.cpp`](/Source/ActionRoguelike/Player/RogueInteractionComponent.cpp): 후보 탐색과 가중치 계산
- [`RogueWorldWidget.cpp`](/Source/ActionRoguelike/Widget/RogueWorldWidget.cpp): 대상 위치에 상호작용 위젯 동기화

저장 시스템은 `URogueSaveComponent`를 부착한 Actor를 GameMode가 자동 수집하는 방식입니다. 

- [`RogueSaveComponent.cpp`](/Source/ActionRoguelike/SaveSystem/RogueSaveComponent.cpp): Actor 데이터 직렬화와 복원
- [`RogueSaveGame.h`](/Source/ActionRoguelike/SaveSystem/RogueSaveGame.h): 저장 데이터 구조
- [`RogueGameMode.cpp`](/Source/ActionRoguelike/Core/RogueGameMode.cpp): 저장 대상 수집과 슬롯 처리

---

## 개발·디버깅 도구

플레이 중 네트워크 관련 기능을 게임 화면에서 관찰하기 위해 `URogueDebugSubsystem : UWorldSubsystem` 기반 로그 Queue와 전용 매크로를 구현했습니다. 

- [`RogueDebugSubsystem.cpp`](/Source/ActionRoguelike/Development/RogueDebugSubsystem.cpp): 화면 로그 큐와 수명 관리
- [`RogueNetUtil.h`](/Source/ActionRoguelike/Development/RogueNetUtil.h): 네트워크 컨텍스트와 디버그 매크로
- [`RogueNetUtil.cpp`](/Source/ActionRoguelike/Development/RogueNetUtil.cpp): CVar 필터 판정과 로그 제출

주요 CVar는 다음과 같습니다.

| CVar | 용도 |
| --- | --- |
| `rogue.net.debug.ShowContext` | 메시지 앞에 PIE/NetMode/Role/Authority/Control 컨텍스트 표시 |
| `rogue.asc.attribute.ShowMsg` | 어트리뷰트 변경 화면 메시지 |
| `rogue.interaction.Debugdraw` | 탐색 반경, 후보 Bounds, 가중치 시각화 |
| `rogue.worldwidget.DebugDraw` | 월드 좌표와 화면 투영 상태 표시 |
| `rogue.projectile.DebugDraw` | 카메라 Trace, 보정 경로, 원래 조준 경로 표시 및 표시 시간 지정 |
| `rogue.ai.minionranged.DebugDraw` | AI 사거리 원과 공격 방향 표시 |
| `rogue.gamemode.spawnbot.ShowDebug` | 생존/최대 Bot 수와 EQS 스폰 결과 표시 |
| `rogue.gamemode.savesystem.ShowDebug` | 저장 성공 여부 표시 |

---

## 투사체

- [`RogueProjectileBase.cpp`](/Source/ActionRoguelike/Projectile/RogueProjectileBase.cpp): 복제, 충돌, 이동, Niagara·Audio와 생성·폭발 효과 등 공통 내용을 담당
- [`RogueProjectileMagic.cpp`](/Source/ActionRoguelike/Projectile/RogueProjectileMagic.cpp): Point Damage와 상태 효과
- [`RogueProjectileBlackhole.cpp`](/Source/ActionRoguelike/Projectile/RogueProjectileBlackhole.cpp): 음수 Radial Force와 Overlap 대상 제거
- [`RogueProjectileTeleport.cpp`](/Source/ActionRoguelike/Projectile/RogueProjectileTeleport.cpp): 충돌·타이머 기반 순간이동

---

## 입력

Enhanced Input Action을 이동·시점·점프·Sprint·세 종류 공격·상호작용·인게임 메뉴에 연결했습니다.

- [`RoguePlayerCharacter.cpp`](/Source/ActionRoguelike/Player/RoguePlayerCharacter.cpp): 이동과 전투 입력
- [`RoguePlayerController.cpp`](/Source/ActionRoguelike/Player/RoguePlayerController.cpp): 상호작용과 메뉴 입력
- [`IMC_DefaultPlayer.uasset`](/Content/ActionRoguelike/Input/IMC_DefaultPlayer.uasset): 기본 키 매핑

---

## UI

`URogueWorldWidget`은 Actor의 월드 위치를 화면 좌표로 투영하고 Viewport Scale을 보정합니다. HUD, 체력바, 효과 아이콘과 상호작용 프롬프트의 레이아웃은 UMG Asset으로 구성했습니다.

- [`RogueWorldWidget.cpp`](/Source/ActionRoguelike/Widget/RogueWorldWidget.cpp): 월드 좌표 투영과 위젯 수명 관리
- [`UI`](/Content/ActionRoguelike/UI): UMG Widget Asset

---

## 에디터 데이터 검증 및 런타임 검증

- [`RogueActionEffect.cpp`](/Source/ActionRoguelike/ActionSystem/RogueActionEffect.cpp): Duration·Period 유효하지 않은 조합 경고
- [`RogueGameMode.cpp`](/Source/ActionRoguelike/Core/RogueGameMode.cpp): Bot Spawn Curve 누락 검증



