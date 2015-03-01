#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <argp.h>
#include "rbtree.h"

#define INDENT_STEP  4

typedef char* (void_to_string)(void* key);

typedef struct {
  int key;
  int value;
} int_key_value;

static int write_node(rbtree_node, FILE*, void_to_string, void_to_string);
static int read_int_pairs(FILE*, int_key_value**);
static int write_int_pairs(int_key_value*, int, FILE*);
static int_key_value* generate_random_key_values_pairs(int);
static char* void_int_to_string(void*);
static int compare_int(void* left, void* right);
static rbtree create_tree_from_pairs(int_key_value*, int);
static void print_tree_values(rbtree_node, int, FILE*);

int write_node(rbtree_node n, FILE* f, void_to_string key_to_string, void_to_string value_to_string) {
  char* color = n->color == BLACK ? ",b\n" : ",r\n";
  char* key = key_to_string(n->key);
  char* value = value_to_string(n->value);
  char* strNode;
  int fputsRetValue;

  assert(key != NULL);
  assert(value != NULL);

  strNode = (char*) malloc((strlen(key) + strlen(value) + 5) * sizeof(char));
  strcpy(strNode, key);
  strcat(strNode, ",");
  strcat(strNode, value);
  strcat(strNode, color);
  fputsRetValue = fputs(strNode, f);
  free(strNode);

  return fputsRetValue;
}

int read_int_pairs(FILE* f, int_key_value** pairsToRet) {
  int nline = 0;
  int v;
  int k;
  char next;

  int_key_value* pairs = (int_key_value*) malloc(100 * sizeof(int_key_value));

  while (1) {
    fscanf(f, "%d,%d", &k, &v);

    next = fgetc(f);
    if (next != '\n') {
      if (next != EOF) {
        return -1;
      } else {
        break;
      }
    }

    pairs[nline].key = k;
    pairs[nline].value = v;

    if ((++nline % 100) == 0) {
      pairs = realloc(pairs, (100 * (100 / nline + 1) * sizeof(int_key_value)));
    }
  }

  *pairsToRet = realloc(pairs, nline * sizeof(int_key_value));
  return nline;
}

int write_int_pairs(int_key_value* pairs, int npairs, FILE* f) {
  int i = 0;
  char* str;

  for (i = 0; i < npairs; i++) {
    str = void_int_to_string((void*) pairs[i].key);
    assert(str != NULL);
    fputs(str, f);
    free(str);
    fputc(',', f);
    str = void_int_to_string((void*) pairs[i].value);
    assert(str != NULL);
    fputs(str, f);
    free(str);
    fputc('\n', f);
  }
}

int_key_value* generate_random_key_values_pairs(num) {
  int i = 0;
  int_key_value* pairs = malloc(num * sizeof(int_key_value));

  for (i = 0; i < num; i++) {
    pairs[i].key = i;
    pairs[i].value = rand();
  }

  return pairs;
}


char* void_int_to_string(void* pval) {
  char *sval = (char*) malloc(50 * sizeof(char));

  if (sprintf(sval, "%d", (int)pval) < 0) {
    free(sval);
    return NULL;
  } else {
    return sval;
  }
}

void write_int_tree_inorder(rbtree_node n, FILE* f) {

  if (n == NULL) {
    return;
  }

  if (n->left != NULL) {
    write_int_tree_inorder(n->left, f);
  }

  assert(write_node(n, f, void_int_to_string, void_int_to_string) > 0);

  if (n->right != NULL) {
    write_int_tree_inorder(n->right, f);
  }
}

int compare_int(void* leftp, void* rightp) {
    int left = (int)leftp;
    int right = (int)rightp;
    if (left < right)
        return -1;
    else if (left > right)
        return 1;
    else {
        assert (left != right);
        return 0;
    }
}

rbtree create_tree_from_pairs(int_key_value* pairs, int npairs) {
  int i = 0;
  rbtree tree = rbtree_create();

  for (i = 0; i < npairs; i++) {
    rbtree_insert(tree, (void*) pairs[i].key, (void*) pairs[i].value, compare_int);
  }

  return tree;
}

void print_tree_values(rbtree_node n, int indent, FILE* f) {
    int i;
    if (n == NULL) {
        return;
    }
    if (n->right != NULL) {
        print_tree_values(n->right, indent + INDENT_STEP, f);
    }

    for(i=0; i<indent; i++) {
        fputs(" ", f);
    }

    if (n->color == BLACK) {
        fprintf(f, "%d\n", (int)n->value);
    } else {
      fprintf(f, "<%d>\n", (int)n->value);
    }

    if (n->left != NULL) {
        print_tree_values(n->left, indent + INDENT_STEP, f);
    }
}


static char doc[] = "Genertate random key-value pairs or provide file with ones to generated a Red-Blak Tree";
static struct argp_option options[] = {
  {"nnodes", 'n', "NUM", 0, "The number of nodes (key-value pairs) to generate"},
  {"input", 'i', "FILE", 0, "The key-value csv file to read, if nnodes is not provided"},
  {"output",  'o', "FILE", 0, "Output to FILE prefix instead of standard output" },
  { 0 }
};

typedef struct {
  char* input;
  char* output;
  int nnodes;
} arguments;

parse_opt (int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
   *      know is a pointer to our arguments structure. */
  arguments* args = state->input;

  switch (key) {
    case 'i':
      args->input = arg;
      break;
    case 'o':
      args->output = arg;
      break;
    case 'n':
      args->nnodes = atoi(arg);

      if (args->input != NULL || args->nnodes < 0) {
        return ARGP_KEY_ERROR;
      }

      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static struct argp argp = { options, parse_opt, NULL, doc };

int main(int argc, char** argv) {
   arguments args ={NULL, NULL, -1};
   int argpRes = argp_parse(&argp, argc, argv, 0, 0, &args);
   int numNodes = 0;
   int_key_value* pairs;
   char *outfilename;
   FILE* out;
   FILE* f;
   rbtree tree;

   switch (argpRes) {
     case ARGP_KEY_ERROR:
       fputs("-n has been defined at the same time than -i or -n is negative number", stderr);
       return 1;
      case ARGP_ERR_UNKNOWN:
       fputs("Non-recognised option", stderr);
       return 1;
   }

   if (args.nnodes > 0) {
     // generate key value parirs
     numNodes = args.nnodes;
     pairs = generate_random_key_values_pairs(numNodes);

     // store them in a file if output is specified
     if (args.output != NULL) {
       outfilename = (char*) malloc((strlen(args.output) + 11) * sizeof(char));
       outfilename = strcat(outfilename, args.output);
       outfilename = strcat(outfilename, "-pairs.csv");
       out = fopen(outfilename, "w");
       free(outfilename);

       if (out == NULL) {
         fprintf(stderr, "error opening pairs output file %s", args.input);
         return 1;
       }

       write_int_pairs(pairs, numNodes, out);
       fclose(out);
     }
   } else {
     // read key value pairs from file
     f = fopen(args.input, "r");

     if (f == NULL) {
       fprintf(stderr, "error opening key-value pairs input file %s", args.input);
       return 1;
     }

     numNodes = read_int_pairs(f, &pairs);

     if (numNodes < 0) {
       fclose(f);
       fprintf(stderr, "error reading key-value pairs input file %s", args.input);
       return 1;
     } else {
       fclose(f);
     }
   }

   // compute red-black tree
   tree = create_tree_from_pairs(pairs, numNodes);
   // pairs are not needed anymore
   free(pairs);

   if (args.output != NULL) {
     // store nodes in a file if output is specified
     outfilename = (char*) malloc((strlen(args.output) + 11) * sizeof(char));
     outfilename = strcpy(outfilename, args.output);
     outfilename = strcat(outfilename, "-nodes.csv");
     out = fopen(outfilename, "w");

     if (out == NULL) {
       fprintf(stderr, "error opening nodes output file %s", args.input);
       return 1;
     }

     write_int_tree_inorder(tree->root, out);
     fclose(out);

     outfilename = strcpy(outfilename, args.output);
     outfilename = strcat(outfilename, "-tree.txt");
     out = fopen(outfilename, "w");
     // output filename is not needed anymore
     free(outfilename);

     if (out == NULL) {
       fprintf(stderr, "error opening tree output file %s", args.input);
       return 1;
     }

     print_tree_values(tree->root, 0, out);
     free(tree);
     fclose(out);
   }

   return 0;
}
