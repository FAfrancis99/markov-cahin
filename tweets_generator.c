#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"
#define FIVE 5
#define FOUR 4
#define USAGE_MSG "USAGE: invalid number of args.\n"
#define ERROR_MSG "ERROR: you can't access to this file.\n"
#define MAX_SEQUENCE_LENGTH 1000
#define THREE 3
#define TWO 2
#define MAX_WORD 20
#define ALLOC_MSG "Allocation failure : can't allocate "
int check_argc (int argc)
{
  if (argc != FIVE && argc != FOUR)
  {
    fprintf (stdout, USAGE_MSG);
    return 1;
  }
  return 0;
}

FILE *open_file (char *file_name)
{
  FILE *fp;
  fp = fopen (file_name, "r");
  if (!fp)
  {
    fprintf (stdout, ERROR_MSG);
    return NULL;
  }
  return fp;
}

int fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  int words_counter = 0;
  char buffer[MAX_SEQUENCE_LENGTH];
  Node *previous_word = NULL;
  while (fgets (buffer, MAX_SEQUENCE_LENGTH, fp) != NULL &&
         words_counter != words_to_read)
  {
    char *new_word = strtok (buffer, " \r\n");
    while (new_word != NULL && words_counter != words_to_read)
    {
      Node *new_node = add_to_database (markov_chain, new_word);
      if(new_node == NULL)
      {
        fprintf (stdout,ALLOC_MSG);
        return EXIT_FAILURE;
      }
      if (previous_word != NULL && previous_word->data->end_of_sequence != 1)
      {
        bool check = add_node_to_counter_list
            (previous_word->data, new_node->data);
        if(!check)
        {
          fprintf (stdout,ALLOC_MSG);
          return EXIT_FAILURE;
        }
      }
      words_counter++;
      previous_word = new_node;
      new_word = strtok (NULL, " \r\n");
    }
  }
  return EXIT_SUCCESS;
}

int main (int argc, char *argv[])
{
  int read_lines = -1, number_tweets, random;
  if (check_argc (argc) == 1)
  {
    return EXIT_FAILURE;
  }
  FILE *fp = fopen (argv[THREE], "r");
  if (fp == NULL)
  {
    return EXIT_FAILURE;
  }
  if (argc == FIVE)
  {
    sscanf (argv[FOUR], "%d", &read_lines);
  }
  sscanf (argv[1], "%d", &random);
  srand (random);
  sscanf (argv[TWO], "%d", &number_tweets);
  LinkedList *link_list = (LinkedList *) calloc
      (1, sizeof (LinkedList));
  MarkovChain *markov_chain = (MarkovChain *)
      malloc (sizeof (MarkovChain));
  if (!markov_chain)
  {
    fprintf (stdout,
             ALLOC_MSG);
    return EXIT_FAILURE;
  }
  markov_chain->database = link_list;
  fill_database (fp, read_lines, markov_chain);
  for (int i = 0; i < number_tweets; ++i)
  {
    MarkovNode *first = get_first_random_node (markov_chain);
    fprintf (stdout, "Tweet %d: ", i + 1);
    generate_random_sequence (markov_chain, first, MAX_WORD);
  }
  free_markov_chain (&markov_chain);
  fclose (fp);
  return EXIT_SUCCESS;
}
