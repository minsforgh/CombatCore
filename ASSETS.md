# Third-Party Assets

이 프로젝트에서 사용 중인 외부 에셋 목록.
라이선스 문제로 저장소에 포함되지 않으므로 직접 다운로드 후 아래 경로에 배치 필요.

---

## 캐릭터 & 애니메이션

### Great Sword Pack (Mixamo)
- **출처**: [Mixamo](https://www.mixamo.com)
- **캐릭터**: Paladin J Nordstrom
- **배치 경로**: `Content/Characters/Paladin/`
- **포함 애니메이션**:
  - great sword slash (1~5) — 경공격 콤보
  - great sword attack — 강공격
  - great sword impact (1~5) — 피격 리액션
  - two handed sword death (1~2) — 사망
  - great sword idle (1~5) — Idle
  - great sword run (1~2) — 이동
  - great sword jump — 점프
  - 기타 전체

---

## 무기 메시

### Runic Sword
- **출처**: [Fab](https://fab.com) — 검색어: `Runic Sword`
- **배치 경로**: `Content/Characters/Paladin/Mesh/SM_Sword.uasset`
- **머티리얼 경로**: `Content/Fab/Runic_Sword/`

---

## 세팅 후 확인 사항

1. Paladin 캐릭터 임포트 시 스켈레톤 이름: `SK_Paladin_Skeleton`
2. 애니메이션 임포트 시 위 스켈레톤 선택 필수
3. Runic Sword 머티리얼 수동 연결 필요 (BaseColor / Normal / Metallic / Roughness)
