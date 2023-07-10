#include <string.h>
#include "markov_chain.h"

// CONSTANTS:

#define END_TWIT_CONST '.'
// the ASCII value of 46

#define SUCSSES_ADD 1

#define MAX_LENGTH_FOR_TWEET 20

#define ERR_MSG_ALLOCATION_FAILURE \
  "Allocation failure: Something went wrong! we couldn't allocate enough "\
  "memory for your program, pleas try again.\n"

// COMPILATION & DECLARATION SECTION:

MarkovNode *create_new_markov_node (char *data_ptr);
int get_random_number (int max_number);
void free_node (Node *cur_del_node);
bool is_not_final_word (char *txt_check);

// ######################################################################### //

Node *add_to_database (MarkovChain *markov_chain, char *data_ptr)
{
//  if node already exists just return it
  Node *node = get_node_from_database (markov_chain, data_ptr);
  if (node != NULL)
  {
    return node;
  }

  MarkovNode *new_markov_node = create_new_markov_node (data_ptr);
 // the printing of the allocation error is being handled.
  if (new_markov_node == NULL)
  { return NULL; }

  int res = add (markov_chain->database, new_markov_node);
  if (res == SUCSSES_ADD)
  { return NULL; }

  return markov_chain->database->last;
}

Node *get_node_from_database (MarkovChain *markov_chain, char *data_ptr)
{
  Node *cur_node = markov_chain->database->first;

  while (cur_node != NULL)
  {
    if (strcmp (cur_node->data->data, data_ptr) == 0)
    { return cur_node; }

    cur_node = cur_node->next;
  }

  return NULL;
}

bool
add_node_to_frequencies_list (MarkovNode *first_node, MarkovNode *second_node)
{
  for (int i = 0; i < first_node->frequencies_list_size; i++)
  {
    if (strcmp (first_node->frequencies_list[i].markov_node->data,
                second_node->data) == 0)
    {
      // increment the frequency if the second_node is already in the
      // frequencies list.
      first_node->frequencies_list[i].frequency++;
      return true;
    }
  }

  // increase the size of the frequencies_list.
  first_node->frequencies_list_size++;
  MarkovNodeFrequency *mnf_ptr = realloc (first_node->frequencies_list,
                                          first_node->frequencies_list_size
                                          * sizeof (MarkovNodeFrequency));

  if (mnf_ptr == NULL)
  {
    // Failed to reallocated memory.
    return false;
  }

  first_node->frequencies_list = mnf_ptr;

  // add the second_node to the frequencies_list.
  first_node->frequencies_list[first_node->frequencies_list_size
                               - 1].markov_node = second_node;
  first_node->frequencies_list[first_node->frequencies_list_size
                               - 1].frequency = 1;

  return true;
}

// vre 1:

//bool add_node_to_frequencies_list (MarkovNode *first_node, MarkovNode
//*second_node)
//{
//  for (int i = 0; i < first_node->frequencies_list_size; i++)
//  {
//    if (strcmp (first_node->frequencies_list[i].markov_node->data,
//                second_node->data) == 0)
//    {
//      first_node->frequencies_list[i].frequency++;
//      return true;
//    }
//  }
//
//  first_node->frequencies_list_size++;
//  if (first_node->frequencies_list_size == 1)
//  {
//    first_node->frequencies_list = malloc (1 * sizeof (MarkovNodeFrequency));
//    if (first_node->frequencies_list == NULL)
//    {
//      return false;
//    }
//  }
//  else
//  {
//    MarkovNodeFrequency *mnf_ptr = realloc (first_node->frequencies_list,
//                                            first_node->frequencies_list_size
//                                            * sizeof (MarkovNodeFrequency));
//    if (mnf_ptr == NULL)
//    {
//      return false;
//    }
//
//    first_node->frequencies_list = mnf_ptr;
//
//  }
//
//  first_node->frequencies_list[first_node->frequencies_list_size - 1]
//      .markov_node = second_node;
//
//  first_node->frequencies_list[first_node->frequencies_list_size - 1]
//      .frequency = 1;
//
//  return true;
//}

void free_database (MarkovChain **ptr_chain)
{
  // defining all the needed Node's.
  Node *cur_del_node = (*ptr_chain)->database->first;
  Node *next_node_to_del;

  while (cur_del_node != NULL)
  {
    next_node_to_del = cur_del_node->next;
    free_node (cur_del_node);

    cur_del_node = next_node_to_del;
  }
}

MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{
  //
  if (markov_chain == NULL)
  {
    return NULL;
  }
  // check if needed.

  Node *node;
  int max_bound;
  int desired_index;

  while (true)
  {
    node = markov_chain->database->first;
    max_bound = markov_chain->database->size;
    desired_index = get_random_number (max_bound);

    for (int i = 0; i < desired_index; i++)
    {
      node = node->next;
    }

    if (!is_not_final_word (node->data->data))
    {
      break;
    }
  }

  return node->data;
}

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  int sigma_frequencies = 0;
  // creat sigma_frequencies.
  for (int j = 0; j < state_struct_ptr->frequencies_list_size; j++)
  {
    sigma_frequencies += state_struct_ptr->frequencies_list[j].frequency;
  }

  // save a pointer to an array of pointers to MarkovNode.
  MarkovNode **rank = calloc (sigma_frequencies, sizeof (void *));
  if (rank == NULL)
  {
    return NULL;
  }

  MarkovNode *temp_node = state_struct_ptr;
  int counter = 0;
  for (int i = 0; i < temp_node->frequencies_list_size; i++)
  {
    for (int k = 0; k < temp_node->frequencies_list[i].frequency; k++)
    {
      rank[k + counter] = temp_node->frequencies_list[i].markov_node;
    }
    counter += temp_node->frequencies_list[i].frequency;
  }

  // get random number.
  int desired_index = get_random_number (sigma_frequencies);
  MarkovNode *node = rank[desired_index];
  free (rank);

  return node;
}

void generate_tweet (MarkovChain *markov_chain, MarkovNode *first_node, int
max_length)
{
  if (first_node == NULL)
  {
    if (markov_chain == NULL)
    {
      return;
    }
    first_node = get_first_random_node (markov_chain);
  }
  MarkovNode *twit_node = first_node;
  int cur_length = 1;

  // printing the twit word by word.
  while ((cur_length < max_length))
  {
    printf ("%s ", twit_node->data);

    if ((twit_node = get_next_random_node (twit_node)) == NULL)
    {
      return; // TODO: check in forom.
    }
    cur_length++;
    if (is_not_final_word (twit_node->data))
    {
      break;
    }
  }
  // printing the twit-last word.
  if (!is_not_final_word (twit_node->data))
  {
    if (cur_length < MAX_LENGTH_FOR_TWEET)
    {
      printf ("%s\n", twit_node->data);
      return;
    }
  }
  printf ("%s\n", twit_node->data);
}

// NEW Function's that were added:

bool is_not_final_word (char *txt_check)
{
  unsigned long length = strlen (txt_check);
  unsigned long last_char_loc = length - 1;

  bool didnt_found_colon = false;

  if (txt_check[last_char_loc] == END_TWIT_CONST)
  {
    didnt_found_colon = true;
  }
  return didnt_found_colon;
}

int get_random_number (int max_number)
{
  return rand () % (max_number);
}

MarkovNode *create_new_markov_node (char *data_ptr)
{
  MarkovNode *new_markov_node = malloc (sizeof (MarkovNode));
  if (new_markov_node == NULL)
  {
    return NULL;
  }
  new_markov_node->data = malloc (strlen (data_ptr) + 1);
  if (new_markov_node->data == NULL)
  {
    free (new_markov_node);
    fprintf (stdout, ERR_MSG_ALLOCATION_FAILURE);
    return NULL;
  }
  strcpy (new_markov_node->data, data_ptr);
  new_markov_node->frequencies_list_size = 0;
  new_markov_node->frequencies_list = NULL;
  return new_markov_node;
}

/**
 * this function is an iner function that deleting all the nodes when is
 * called.
 *
 * this function is freeing one node.
 */
void free_node (Node *cur_del_node)
{
  // freeing the frequencies_list.
  free (cur_del_node->data->frequencies_list);
  cur_del_node->data->frequencies_list = NULL;

  // freeing the string.
  free (cur_del_node->data->data);
  cur_del_node->data->data = NULL;

  // freeing the data.
  free (cur_del_node->data);
  cur_del_node->data = NULL;

  // freeing the main node that is holding all the data.
  free (cur_del_node);
}