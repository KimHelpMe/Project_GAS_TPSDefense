# GASCC

**GAS 기반 협동 TPS 타워 디펜스 (Unreal Engine 5)**

> Paragon 영웅 중 하나를 골라 팀을 이루고, 경로를 따라 몰려오는 적과 보스로부터 본진을 지키는 Steam 멀티플레이 협동 액션입니다.
> 처치한 적이 떨어뜨리는 스킬을 주워 능력을 조합하고, 본진이 무너지기 전에 웨이브를 버팁니다.

---

## 게임 소개

- **캐릭터** — Twinblast / LtBelica / Murdock / Boris / Revenant. 각자 고정 기본공격 + 스킬 슬롯(Q/E/R/T)
- **스킬 드랍 & 픽업** — 적 처치 시 웨이브 가중치에 따라 스킬 박스가 스폰. 조준해서 주우면 슬롯에 장착, 레벨업·쿨다운·코스트 존재
- **전투** — 4방향 피격 반응, 투사체 범위 데미지, 샷건 거리 감쇠, 팀 태그로 아군 오사 차단
- **타워 디펜스** — 적은 Spline 경로를 따라 본진으로 이동, 구조물이 경로를 막고 피해를 흡수
- **생존 규칙** — 사망 시 리스폰 카운트다운, 보스 처치 시 승리 / 본진 파괴 시 게임오버
- **협동 편의** — 시야 차단물 자동 페이드, 화면 고정 보스 체력바, 인게임 채팅

## 게임 흐름

```
타이틀 → 로비(캐릭터 선택 · 채팅) → [전환 연출] → 인게임 웨이브 → 승리/패배 → 로비 복귀
```

---

## 기술 스택

| 분류 | 내용 |
|------|------|
| 엔진 / 언어 | Unreal Engine 5 · C++ |
| 게임플레이 | Gameplay Ability System (Attribute / Effect / Ability / Tag / Cue), Behavior Tree |
| 네트워크 | Steam Online Subsystem + `Plugins/MultiplayerSessions`, Seamless Travel |
| 에셋 | Epic Games Paragon (Unreal Marketplace 무료 배포분) |

## 빌드 / 실행

1. Unreal Engine 5 설치 후 `GASBase.uproject` 우클릭 → *Generate Visual Studio project files*
2. Visual Studio에서 빌드 → 에디터 실행
3. 멀티플레이 테스트: Steam 클라이언트 실행 상태에서 Standalone 인스턴스 2개 이상 (`Number of Players` ≥ 2, Net Mode: *Play As Client*)

---

## 스크린샷 / GIF

<!-- docs/ 에 게임플레이 이미지 추가 -->

---

## 팀

부트캠프 팀 프로젝트입니다. 파트별로 나눠 작업했으며, 담당은 `git log` authorship으로 확인할 수 있습니다.

| 파트 | 담당 |
|------|------|
| GAS 코어 · 전투 · 사망/리스폰 · 이동 · 로비/세션 · 레벨 전환 | csk9914 |
| AI · 스테이지 · 그 외 시스템 | 팀원 |

캐릭터·애니메이션 에셋은 Epic Games Paragon 무료 배포분이며 상용 배포 목적이 아닙니다.

<sub>파트별 담당은 커밋 히스토리(`git log`, GitHub `?author=` 필터)로 확인할 수 있습니다.</sub>
