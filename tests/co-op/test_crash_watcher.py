import json
from http.server import BaseHTTPRequestHandler, HTTPServer
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import threading
import unittest


class CrashWatcherTest(unittest.TestCase):
    def test_only_new_crashes_notify_without_artifact_contents(self):
        received = []
        notified = threading.Event()

        class Handler(BaseHTTPRequestHandler):
            def do_POST(self):
                received.append(json.loads(self.rfile.read(int(self.headers["Content-Length"]))))
                self.send_response(200)
                self.end_headers()
                self.wfile.write(b"{}")
                notified.set()

            def log_message(self, *_):
                pass

        server = HTTPServer(("127.0.0.1", 0), Handler)
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        script = Path(__file__).resolve().parents[2] / "tools/co-op/watch-crashes.py"
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            old = directory / "Exceptions/exception-old/except.txt"
            old.parent.mkdir(parents=True)
            old.write_text("old private report")
            import os
            environment = os.environ.copy()
            environment["OPENTS_REPORT_URL"] = f"http://127.0.0.1:{server.server_port}/report"
            process = subprocess.Popen(
                [sys.executable, str(script), temporary, "--build", "test-head"],
                env=environment, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True,
            )
            try:
                self.assertIn("Watching", process.stdout.readline())
                self.assertEqual(received, [])
                new = directory / "Exceptions/exception-new/except.txt"
                new.parent.mkdir(parents=True)
                new.write_text("password=secret; private filesystem paths")
                self.assertTrue(notified.wait(5), "the new artifact must cause a real HTTP notification")
                self.assertEqual(len(received), 1)
                self.assertEqual(received[0]["lane"], "opents")
                self.assertIn("test-head", received[0]["source"])
                self.assertNotIn("password", json.dumps(received))
                self.assertNotIn(temporary, json.dumps(received))
                process.send_signal(signal.SIGINT)
                process.communicate(timeout=5)
                self.assertEqual(process.returncode, 0)
            finally:
                if process.poll() is None:
                    process.kill()
                    process.communicate()
                server.shutdown()
                server.server_close()
                thread.join()


if __name__ == "__main__":
    unittest.main()
