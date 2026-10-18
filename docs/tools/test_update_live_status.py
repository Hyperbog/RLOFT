"""Offline regression tests for factual, non-acceptance status generation."""
import unittest
from pathlib import Path
from unittest.mock import patch
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
import update_live_status as live


class StatusTests(unittest.TestCase):
    def test_render_distinguishes_ci_from_game_acceptance(self):
        doc = live.render({
            "default_branch": "code1-decompilation",
            "items": [{
                "label": "Playable enhanced game",
                "branch": "demo-v1",
                "sha": "a" * 40,
                "last_run": {
                    "id": 42,
                    "html_url": "https://github.com/Hyperbog/RLOFT/actions/runs/42",
                    "head_sha": "b" * 40,
                    "conclusion": "success",
                },
            }],
            "prs": [],
            "additional": [],
        }, "2026-10-17 00:00:00")
        self.assertIn("most recent *completed*", doc)
        self.assertIn("not proof of gameplay", doc)
        self.assertIn("a" * 10, doc)
        self.assertIn("b" * 10, doc)
        self.assertIn("#42", doc)
        self.assertIn("No open pull requests", doc)

    def test_collect_resolves_pending_default_branch_rename(self):
        actual = [names[-1] for _, names in live.WORKSTREAMS]
        branch_payload = [{"name": n, "commit": {"sha": "f" * 40}} for n in actual]
        def fixture(path, token, repo):
            if path == "":
                return {"default_branch": "code1-decompilation"}
            if path.startswith("branches?"):
                return branch_payload
            if path.startswith("pulls?"):
                return []
            if path.startswith("actions/runs?"):
                return {"workflow_runs": []}
            raise AssertionError(path)
        with patch.object(live, "api", side_effect=fixture):
            result = live.collect("test-token")
        self.assertEqual(len(result["items"]), 7)
        self.assertEqual(result["items"][0]["branch"], "code1-decompilation")
        self.assertFalse(result["additional"])

    def test_rejects_missing_owner_branch(self):
        with patch.object(live, "api", side_effect=[
            {"default_branch": "code1-decompilation"}, [], []
        ]):
            with self.assertRaisesRegex(ValueError, "Required workstream"):
                live.collect("test-token")

    def test_escape_untrusted_title(self):
        self.assertEqual(live.markdown("bad | heading\nnew"), "bad \\| heading new")


if __name__ == "__main__":
    unittest.main()
