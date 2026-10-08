# Optional AI Disassembly Assistant — design decision and research (2026-10-08)

**STATUS: DEFERRED UNTIL AFTER THE FINAL RELEASE — NOT IN CURRENT DEVELOPMENT SCOPE.** Per the project owner's instruction of 2026-10-08, the AI assistant will not be prototyped, implemented, tested, packaged, integrated, or prioritized during the disassembler's initial development, beta, release-candidate or final-release cycle. No AI-related UI, credentials, SDKs, service wiring, provider integrations or scaffolding should be introduced until *after* the fully functional disassembler's final release is delivered and accepted, and the owner explicitly chooses to reopen this idea.

**This document is a retained future-reference research note, not an active implementation specification or backlog.** The API/product details below are a 2026-10-08 snapshot and **must be reverified** when the idea is revisited. No provider credentials, network calls, or LLM dependencies are needed for DOSDis analysis. No automatic source-language generation is in scope.

## Product and user experience

A dockable "AI Assistant" panel provides streamed multi-turn chat associated with an immutable project and optional currently selected function, instruction span, data item, or search results. It can request additional bounded local evidence through typed tools. The user can ask follow-ups and refine tasks without pasting assembly manually. Local history and tool/evidence references persist with the project.

Suggested starter actions: explain selection; inspect incoming/outgoing xrefs; identify strings/jump tables; locate probable function boundaries; compare routines; suggest labels/comments; summarize File Intelligence evidence; and list remaining uncertainties. Suggestions are **candidates**, not verified disassembly or classifications.

## Provider-neutral interfaces

`IAiProvider`: authenticate/connect/disconnect, list available models and model capabilities, stream responses, tool-call roundtrips, cancellation, usage reporting, structured errors.
`IAiConversationStore`: persist local chat, request/response references, attached artifact SHA-256, tool inputs and outputs, model and provider.
`IAiContextBuilder`: construct bounded, structured context from current project; count size, show redaction/preview and provenance.
`IAiToolBroker`: validate tool schema, scope and budget; call local read-only analysis APIs; separate proposed writes from approved writes.
`IAiProposalManager`: store suggested symbols, comments, code/data assessments and matches with evidence and diffs; approve/reject/undo without changing original game files.

Provider backends should be independent optional adapters, not dependencies of DosDis.Core. Initial candidate backends:

- **ChatGPT plan OAuth:** OpenAI "Sign in with ChatGPT" for eligible open-source/local apps. User explicitly authorizes account/plan usage; OAuth Authorization Code + PKCE and stable host ID; discover account-available models with `GET /v1/models`. Does not grant access to their existing ChatGPT conversations, saved context, or API key. Eligibility/approval for a particular distributable app must be checked.
- **OpenAI API key:** ordinary OpenAI Responses API with separate API billing; model and reasoning controls from provider-supported capabilities.
- **Other hosted APIs:** Anthropic and Google Gemini through dedicated adapters, subject to current public contracts.
- **Local/offline:** Ollama or LM Studio and other user-configured compatible endpoints. Capabilities such as streaming, tool calls, and thinking levels vary by model and server; query or negotiate rather than assume.

**Do not** use browser cookie extraction, ChatGPT private backend endpoints, credential copying from the ChatGPT desktop client, or unsupported subscription workarounds.

## Specific ChatGPT-plan integration constraints (OpenAI's 2026 preview)

- Official docs: https://developers.openai.com/siwc/token-sharing-open-source ; https://developers.openai.com/siwc/token-sharing-open-source/models-and-inference ; https://developers.openai.com/siwc/token-sharing-open-source/preview-limitations
- Use the system browser and local OAuth callback; CSRF state + nonce + PKCE; validate tokens/scopes, persist stable opaque host ID and issued client registration, securely refresh credentials.
- Bearer token targets **public** `https://api.openai.com/v1/responses`, not ChatGPT backend APIs; model picker uses account-specific public `/v1/models`.
- Current HTTP preview requires `store:false`, `stream:true`, explicit conversation context in `input` per turn and **no** HTTP `previous_response_id`. Local conversation history must be independently persisted; app follows supported function/custom-tool flow.
- No hosted file search, Code Interpreter, native computer use, hosted MCP/connectors or tool_search on this specific plan-sharing route as of this review. Design all analysis tools client-side and self-contained; other provider modes may offer different features.
- ChatGPT plan allowances and UI model labels are not guaranteed identical to commercial API entitlements. Display only model and reasoning choices supported by the selected account/model.
- If released as a private/commercial application, verify SIWC eligibility/application approval; the API-key provider remains a reliable independent option.

## Context and read-only tool contracts

Use typed object-offset addresses with artifact SHA-256 and verified address-space mappings. The model **never** receives an uncontrolled local filesystem path or unsanitized unbounded archive dump.

Candidate read-only tools: `get_artifact_metadata`, `get_instruction_range`, `get_function_cfg`, `get_callers`, `get_callees`, `get_xrefs`, `read_data_bytes`, `search_strings`, `list_relocations`, `get_file_intelligence`, `compare_functions`, `get_evidence_ledger`. Return bounded arrays including unknowns and confidence classifications.

AI output is untrusted. It may propose labels, comments, hypotheses, comparison records and optional follow-up reads. These never override analyst-verified facts or byte/classification evidence. All proposed write actions require explicit human approval, display their exact diff and original evidence, and support undo. Avoid arbitrary command execution or code patching in the initial AI feature.

## Privacy, cost and policy

- Feature disabled by default, deliberate provider selection and connect action, clear display of connected account / active model / effort / budgets.
- **No automatic upload** of game binaries, whole archives, user notes, or private repositories. Show request preview, provenance, estimated size and data-destination warning; select only required snippets. User consent and documented provider retention policy are needed before transmitting proprietary text/bytes.
- Store local API keys/OAuth credentials in Windows DPAPI/Credential Manager, never SQLite project exports or logs. Support account disconnect/revocation and explicit deletion of local conversation cache.
- Host settings: model selector, compatible effort values, per-task max tool reads, per-request token/cost thresholds (when provider exposes prices), timeout, cancellation, streaming, selected privacy mode, optional manual context attachment.
- Prompt-injection resilience: treat strings/annotations/resources as untrusted task data; reject instructions embedded in binaries or scraped documents to run tools, reveal secrets or change policies.
- All ordinary disassembly continues fully offline and deterministically if no AI provider is selected or if it fails.

## Possible post-final-release sequence (inactive; do not schedule before final release)

1. **AI-00 contract**: provider/capability registry and local conversation/event schema, fully isolated from binary loaders; no network by default.
2. **AI-01 read-only chat**: optional WPF panel, streamed multi-turn text, selection-aware context previews, mock provider and cancellation tests.
3. **AI-02 OpenAI API adapter**: secure BYO-API-key auth, explicit model/effort capabilities, conversation roundtrips; test with approved live account.
4. **AI-03 ChatGPT sign-in**: official SIWC eligibility review, OAuth PKCE, account-specific model list, preview-compliant streaming/explicit history, tool-call testing.
5. **AI-04 local and external providers**: LM Studio/Ollama, Anthropic/Gemini; separate adapters and tested capability discovery.
6. **AI-05 evidence-linked suggestions**: diffed annotation proposals, manual approve/reject/undo and deterministic replay fixtures.
7. **AI-06 adversarial tests**: injection, data exfiltration prevention, consent/cost budgets, credential storage, network failure and model/tool disagreement.
8. **AI-07 benchmark**: compare usefulness against known Ravenloft functions; monitor *false verified claims* and ensure AI never upgrades an unknown/candidate result to verified by itself.

AI is not part of the automatic analyzer's fidelity certificates. Only original-byte and independently validated executable evidence can certify assembly conclusions.

## Windows-only build policy

Per the project user's 2026-10-08 request, only Windows x64 builds/tests and releases are produced. Linux CI, Linux self-contained publishing and Linux release artifacts are discontinued until separately requested.
