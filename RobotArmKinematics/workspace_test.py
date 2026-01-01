import time
import json
import kinematics as k

# link lengths in mm
base_link_length = 50
l1, l2 = 110, 71
general_offset = 60 

# Collection for analysis
results = {
    "metadata": {
        "links": [l1, l2],
        "base_height": base_link_length,
        "offset": general_offset
    },
    "successful_points": [],
    "failed_summary": {
        "out_of_reach_or_math_error": 0
    }
}

# Grid parameters
STEP_SIZE = 10 
MAX_REACH = l1 + l2 + 10 # 191mm

print(f"Starting Workspace Analysis (Step: {STEP_SIZE}mm)...")
start_time = time.time()

# Iterate through the 3D grid
for x in range(-MAX_REACH, MAX_REACH + 1, STEP_SIZE):
    for y in range(0, MAX_REACH + base_link_length + 1, STEP_SIZE):
        for z in range(-MAX_REACH, MAX_REACH + 1, STEP_SIZE):
            
            angles = k.IK(base_link_length, l1, l2, x, y, z, general_offset)
            
            if angles:
                results["successful_points"].append({
                    "coord": [x, y, z],
                    "angles": angles
                })
            else:
                results["failed_summary"]["out_of_reach_or_math_error"] += 1

# Specific test for your problematic point
test_point = [0, 182, 0]
test_result = k.IK(base_link_length, l1, l2, *test_point, general_offset)
results["specific_test"] = {
    "point": test_point,
    "result": test_result
}

# Save to file
filename = "workspace_results.json"
with open(filename, "w") as f:
    json.dump(results, f, indent=4)

end_time = time.time()
print("--- Analysis Complete ---")
print(f"Data saved to {filename}")
print(f"Successes: {len(results['successful_points'])}")
print(f"Failures: {results['failed_summary']['out_of_reach_or_math_error']}")
print(f"Time elapsed: {round(end_time - start_time, 2)} seconds")
print(f"Specific Test [0, 182, 0]: {'SUCCESS' if test_result else 'FAILED'}")