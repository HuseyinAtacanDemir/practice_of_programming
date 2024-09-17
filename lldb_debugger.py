def print_list_item(debugger, list_item_ptr):
    # Iterate through the linked list
    while int(list_item_ptr.GetValue(), 0) != 0:
        # Get the HashmapItem struct from the ListItem's data pointer
        hashmap_item_ptr = list_item_ptr.GetChildMemberWithName("data").Cast(
            debugger.GetSelectedTarget().FindFirstType('HashmapItem').GetPointerType()
        )

        if not hashmap_item_ptr.IsValid():
            print("Error: Invalid HashmapItem pointer.")
            return
        
        # Cast the void *data pointer in HashmapItem to a Person * type
        person_ptr = hashmap_item_ptr.GetChildMemberWithName("data").Cast(
            debugger.GetSelectedTarget().FindFirstType('Person').GetPointerType()
        )

        if not person_ptr.IsValid():
            print("Error: Invalid Person pointer.")
            return

        # Extract person fields
        fname = person_ptr.GetChildMemberWithName("fname").GetSummary().strip('"') if person_ptr.GetChildMemberWithName("fname").GetSummary() else "Unknown"
        lname = person_ptr.GetChildMemberWithName("lname").GetSummary().strip('"') if person_ptr.GetChildMemberWithName("lname").GetSummary() else "Unknown"
        ssn = person_ptr.GetChildMemberWithName("ssn").GetValueAsUnsigned()

        # Print person details
        print(f"| {fname}, {lname}, {ssn} |", end="")

        # Move to the next ListItem
        next_ptr = list_item_ptr.GetChildMemberWithName("next")
        list_item_ptr = next_ptr

        # Indicate if the next ListItem is NULL
        if int(next_ptr.GetValue(), 0) == 0:
            print(" ---> NULL", end="")
        else:
            print(" --->", end="")

    print("")  # New line at the end of the list


def print_hashmap(debugger, command, result, internal_dict):
    # Get the current frame to access local variables
    target = debugger.GetSelectedTarget()
    frame = target.GetProcess().GetSelectedThread().GetSelectedFrame()
    
    # Find the Hashmap variable (change 'hmap' if needed to the actual variable name in use)
    hashmap = frame.FindVariable("hmap")

    # Ensure the hashmap is valid
    if not hashmap.IsValid():
        print("Error: Invalid hashmap variable.")
        return

    # Get the number of buckets in the hash map
    n_bucket = hashmap.GetChildMemberWithName("n_bucket").GetValueAsUnsigned()

    # Get the table, which is an array of ListItem pointers
    table = hashmap.GetChildMemberWithName("table")

    print("------------ Hash Map Visualizer ------------")

    # Iterate over each bucket in the hash map
    i = 0
    while i < n_bucket:
        bucket_ptr = table.GetValueForExpressionPath(f"[{i}]")

        if int(bucket_ptr.GetValue(), 0) != 0:  # If the bucket is not empty
            print(f"---------------")  # Single horizontal line for consistency
            print(f"|  hash {i:<4}  |---->", end="")
            print_list_item(debugger, bucket_ptr)
            i += 1
        else:
            # Handle consecutive NULL buckets
            start_null = i
            while i < n_bucket and int(table.GetValueForExpressionPath(f"[{i}]").GetValue(), 0) == 0:
                i += 1
            end_null = i - 1

            if start_null == end_null:
                # Print a single NULL bucket
                print(f"---------------")
                print(f"|  hash {start_null:<4}  |----> NULL")
            else:
                # Print a range of consecutive NULL buckets with fixed-width format
                print(f"---------------")
                print(f"| {start_null:>5}..{end_null:<5}|----> NULL")

    print("---------------")  # Ensure no intersection with the end banner
    print("------------ End of Hash Map ------------")


def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand('command script add -f ' + __name__ + '.print_hashmap print_hashmap')
    print("The 'print_hashmap' command has been added to LLDB.")

