# MOBA_Project

Unreal Engine 5.6 기반의 멀티플레이어 MOBA 게임 프로젝트입니다.  
Gameplay Ability System(GAS), AI, 네트워크 리플리케이션, MVVM 기반 UI 등 UE5의 핵심 기술들을 활용하여 구현되었습니다.

---

## 🛠️ 기술 스택

| 항목 | 내용 |
|------|------|
| **엔진** | Unreal Engine 5.6 |
| **언어** | C++ |
| **주요 플러그인** | GameplayAbilities, CommonUI, ModelViewViewModel, EnhancedInput, OnlineSubsystem |

---

## 📁 프로젝트 구조

```
Source/MOBA_Project/
├── Private/
  ├── AI/                  # AI 컨트롤러, 미니언 배럭, BT 태스크
  ├── Animations/          # 애님 노티파이, 애님 인스턴스
  ├── Character/           # 기본 캐릭터, 플레이어 캐릭터, 미니언
  ├── Components/Input/    # 커스텀 인풋 컴포넌트
  ├── Controllers/         # 메인메뉴, 로비, 게임플레이 컨트롤러
  ├── DataAssets/          # 캐릭터 정의, 아이템, 입력 설정, 스타트업 데이터
  ├── Framework/           # 캡처포인트, 게임인스턴스, 게임스테이트, 에셋매니저
  ├── GameModes/           # 게임플레이, 로비, 메인메뉴 게임모드
  ├── GAS/                 # 어빌리티 시스템 전반
  │   ├── Abilities/       # AOE, Combo, Dash, Freeze, Shoot, Dead, Launched
  │   ├── MMC/             # 커스텀 모디파이어 계산 (기본 공격 데미지)
  │   └── (ASC, AttributeSet, Projectile, TargetActor 등)
  ├── InventoryItem/       # 인벤토리 컴포넌트 & 아이템
  ├── MTypes/              # 열거형, 구조체 타입 정의
  ├── Network/             # 게임 세션, 네트워크 유틸
  ├── Player/              # 플레이어 스테이트
  └── Widgets/             # UI 위젯 및 MVVM ViewModel


```

---

## ⚙️ 핵심 시스템

### 1. Gameplay Ability System (GAS)
UE5의 GAS를 기반으로 캐릭터의 능력치와 스킬을 관리합니다.

- **AttributeSet**: HP, MaxHP, MP, MaxMP, 공격력, 방어력, 이동 속도 등 리플리케이션 지원
- **어빌리티 목록**
  - `GA_Combo` : 콤보 근접 공격
  - `GA_Dash` : 대시 이동
  - `GA_Shoot` : 원거리 투사체 발사
  - `GA_AOE` : 범위 스킬
  - `GA_Freeze` : 빙결 CC기
  - `GAP_Dead` / `GAP_Launched` : 패시브 (사망, 공중 피격)
- **타겟팅**: `TargetActor_Around`(주변 범위), `TargetActor_GroundPick`(지면 지정)
- **MMC**: `MMC_BaseAttackDamage` — 기본 공격 데미지 커스텀 계산

### 2. 캐릭터 시스템
- `AMCharacter` : 모든 캐릭터의 베이스 클래스. GAS, 팀 ID, 사망/부활, AI 인식, 오버헤드 UI 포함
- `AMPlayerCharacter` : 플레이어 전용 확장 클래스
- `AMinion` : AI가 제어하는 미니언 유닛

### 3. AI 시스템
- `MAIController` : 플레이어 추적 및 어빌리티 사용 AI
- `MinionBarrack` : 주기적으로 미니언을 스폰하는 배럭 액터
- `BTTask_ActivateAbilityByTag` : 비헤이비어 트리에서 GAS 어빌리티를 태그로 실행하는 커스텀 태스크

### 4. 캡처 포인트
팀 간 거점 점령 시스템입니다.

- 점령 상태: `Neutral → Capturing → Captured → Contested`
- 팀별 색상으로 Niagara 이펙트 실시간 반영
- 점령 후 일정 시간 유지 시 승리 선언
- 모든 상태가 서버 리플리케이션으로 동기화

### 5. 네트워크 & 멀티플레이어
- `MGameSession` : 플레이어 등록/해제, AutoLogin 처리
- `MNetStatics` : 네트워크 관련 정적 유틸 함수 모음
- `MPlayerState` / `MGameState` : 게임 진행 상태 리플리케이션

### 6. 인벤토리 & 상점
- `InventoryComponent` : 캐릭터에 부착되는 인벤토리 컴포넌트
- `InventoryItem` : 개별 아이템 데이터
- `ShopSubsystem` : 상점 로직 관리 서브시스템
- UI: 드래그&드롭, 컨텍스트 메뉴, 트리 구조 아이템 뷰 지원

### 7. UI / MVVM
CommonUI + UE5 ModelViewViewModel 플러그인 기반의 UI 시스템입니다.

| ViewModel | 역할 |
|-----------|------|
| `MVVM_Gauge` | HP/MP 게이지 |
| `MVVM_HeroStats` | 영웅 스탯 표시 |
| `MVVM_AbilityEntry` | 어빌리티 슬롯 |
| `MVVM_CapturePoint` | 캡처 포인트 상태 |
| `MVVM_InventorySlot` | 인벤토리 슬롯 |
| `MVVM_ShopItem` | 상점 아이템 |


