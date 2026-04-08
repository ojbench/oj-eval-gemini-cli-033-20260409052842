import os
import sys
import json
sys.path.append("/workspace/problem_033/submit_acmoj")
from acmoj_client import ACMOJClient

client = ACMOJClient(os.environ.get("ACMOJ_TOKEN"))
result = client.submit_git(2100, "https://github.com/ojbench/oj-eval-gemini-cli-033-20260409052842")
print(json.dumps(result))
