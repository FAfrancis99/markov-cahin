#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "markov_chain.h"
#define ALLOC_MSG "Allocation failure : can't allocate "
#define MAX_WORDS 20
/**
* Get random number between 0 and max_number [0, max_number).
* @param max_number maximal number to return (not including)
* @return Random number
*/
int get_random_number (int max_number)
{
  return rand () % max_number;
}

MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{
  Node *ptr;
  int flag = 1;
  while (flag == 1)
  {
    ptr = markov_chain->database->first;
    int number = get_random_number (markov_chain->database->size);
    for (int i = 0; i < number; i++)
    {
      ptr = ptr->next;
    }
    flag = ptr->data->end_of_sequence;
  }
  return ptr->data;
}

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  int rand_num = get_random_number (state_struct_ptr->
      num_of_appearance);
  for (int i = 0; i < state_struct_ptr->counter_list_len; ++i)
  {
    rand_num -= state_struct_ptr->counter_list[i].frequency;
    if (rand_num < 0)
    {
      return state_struct_ptr->counter_list[i].markov_node;
    }
  }
  return NULL;
}

void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  if (first_node == NULL)
  {
    first_node = get_first_random_node (markov_chain);
  }
  char sequence[MAX_WORDS * MAX_WORD_LENGTH] = "";
  int i = 0;
  while (i < max_length && first_node->end_of_sequence == 0)
  {
    strcat (sequence, first_node->data);
    strcat (sequence, " ");
    first_node = get_next_random_node (first_node);
    ++i;
  }
  if (i < max_length)
  {
    strcat (sequence, first_node->data);
  }
  strcat (sequence, "\n");
  fprintf (stdout, "%s", sequence);
}

void free_markov_chain (MarkovChain **ptr_chain)
{
  Node *node = (*ptr_chain)->database->first;
  Node *temp = NULL;
  for (int i = 0; i < (*ptr_chain)->database->size; i++)
  {
    free (node->data->counter_list);
    free (node->data->data);
    free (node->data);
    temp = node;
    node = node->next;
    free (temp);
    temp = NULL;
  }
  free ((*ptr_chain)->database);
  (*ptr_chain)->database = NULL;
  free (*ptr_chain);
  *ptr_chain = NULL;

}

int word_in_list (MarkovNode *first_node, MarkovNode *second_node)
{
  for (int i = 0; i < first_node->counter_list_len; ++i)
  {
    if (strcmp (first_node->counter_list[i].markov_node->data,
                second_node->data) == 0)
    {
      first_node->counter_list[i].frequency++;
      return 0;
    }
  }
  return 1;
}
bool add_node_to_counter_list (MarkovNode *first_node, MarkovNode *second_node)
{
  if (first_node->counter_list == NULL && first_node->end_of_sequence != 1)
  {
    NextNodeCounter *list = (NextNodeCounter *) malloc
        (sizeof (NextNodeCounter));
    if (!list)
    {
      fprintf (stdout,
               ALLOC_MSG);
      return false;
    }
    list[0].markov_node = second_node;
    list[0].frequency = 1;
    first_node->counter_list = list;
    first_node->counter_list_len++;
  }
  else
  {
    if (word_in_list (first_node, second_node) == 1)
    {
      NextNodeCounter *temp = realloc (first_node->counter_list,
                                       (first_node->counter_list_len+1)
                                       * sizeof (NextNodeCounter));
      if (!temp)
      {
        fprintf (stdout,
                 ALLOC_MSG);
        return false;
      }
      temp[first_node->counter_list_len].markov_node = second_node;
      temp[first_node->counter_list_len].frequency = 1;
      first_node->counter_list = temp;
      first_node->counter_list_len++;
    }
  }
  first_node->num_of_appearance++;
  return true;
}

Node *get_node_from_database (MarkovChain *markov_chain, char *data_ptr)
{
  if (markov_chain->database->size == 0)
  {
    return NULL;
  }
  Node *ptr = markov_chain->database->first;
  while (ptr)
  {
    if (strcmp (ptr->data->data, data_ptr) == 0)
    {
      return ptr;
    }
    ptr = ptr->next;
  }
  return NULL;
}

Node *add_to_database (MarkovChain *markov_chain, char *data_ptr)
{
  Node *new = get_node_from_database (markov_chain, data_ptr);
  if (new == NULL)
  {
    MarkovNode *new_word = (MarkovNode *) calloc
        (1, sizeof (MarkovNode));
    if (!new_word)
    {
      fprintf (stdout, ALLOC_MSG);
      return NULL;
    }
    new_word->data = (char *) malloc (strlen (data_ptr) + 1);
    if (!new_word->data)
    {
      fprintf (stdout, ALLOC_MSG);
      return NULL;
    }
    strcpy (new_word->data, data_ptr);
    if (strcmp (&new_word->data[strlen (new_word->data) - 1], ".") == 0)
    {
      new_word->end_of_sequence = 1;
    }
    int if_added = add (markov_chain->database, new_word);
    if (if_added == 1)
    {
      free (new_word);
      new_word = NULL;
      fprintf (stdout, ALLOC_MSG);
      return NULL;
    }
    return markov_chain->database->last;
  }
  return new;
}
