#pragma once

// log definition
#define error_log(...) fprintf(stderr, __VA_ARGS__)  
#define debug_log(...) fprintf(stdout, __VA_ARGS__) 
#define print_info(...) fprintf(stdout, __VA_ARGS__) 