# CLAUDE.md

> 상세 규칙: `.claude/architecture.md` / 스킬 정의: `.claude/skills/`

## 프로젝트 메타

- **엔진**: Unreal Engine 5.6 C++ — Epic Coding Standard 준수
- **AI 플러그인**: StateTree (Behavior Tree 미사용)
- **구조**: 세 Variant 독립 스택 — Combat / Platforming / SideScrolling

## 핵심 규칙

1. **Zero AI Metadata** — 소스 코드에 AI 주석/태그 금지
2. **클래스 생성은 사용자 담당** — Claude는 로직 구현만
3. **spec 없이 Full 워크플로우 구현 시작 금지** — `/new-spec` 선행 필수
4. **My Draft 먼저** — 처음 접하는 로직은 내가 뼈대 작성 후 피드백 요청
5. **git commit 메시지에 AI 메타데이터 금지** (co-author, AI agent 등)

## 워크플로우 요약

| 분류 | 순서 |
|---|---|
| Full (새 기능) | `/new-spec` → Plan → 구현 → `/simplify` → Comprehension Check → `/sync-progress` |
| Light (수정) | Plan → 구현 → `/simplify` → `/sync-progress` |
| Hotfix (버그) | `/debug` → 수정 → `/sync-progress` |

## 세션 시작 규칙

매 세션 시작 시 아무 작업 전에:
1. 아래 "현재 진행 상황"을 읽는다
2. 진행 중인 spec이 있으면 해당 파일을 열어 남은 단계 확인
3. "지난 세션에서 [X]까지 완료했고, 다음은 [Y]입니다" 형태로 브리핑

## 현재 진행 상황

- Phase: 2 - 콤보 시스템
- 마지막 완료: spec-004 UComboDataAsset + 경3타 콤보 + 강공격 분기
- 진행 중: -
- 다음: spec-005 UInputBufferComponent (시간 기반 만료)
