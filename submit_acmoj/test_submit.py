import os
import sys
sys.path.append('/workspace/problem_033/submit_acmoj')
from acmoj_client import ACMOJClient

client = ACMOJClient(os.environ.get("ACMOJ_TOKEN"))
print(dir(client))
