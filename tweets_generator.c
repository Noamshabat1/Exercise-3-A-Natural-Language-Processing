#include "markov_chain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define WORD_MAX_LENGTH 100
#define BUFFER_SIZE 1000
#define FULL_AMOUNT_OF_ARGC 5
#define ACCEPTED_AMOUNT_OF_ARGC 4
#define TEMP_NUMBER (-100)
#define DELIMITERS "\n\r\t "

typedef enum Program
{
    SEED = 1,
    TWEETS_NUMBER,
    TEXT_CORPUS_PATH,
    WORD_TO_READ
} Program;


// ERROR MESSAGE'S SECTION:

#define ERR_MSG_FILE_PATH "Error: the program have an invalid file path.\n"

#define ERR_MSG_USAGE_PROBLEM "Usage: Please fill the following command's \
./tweets_generator_logic <seed> <number of tweets> <text corpus path> \
[words to read].\n"

#define ERR_MSG_ALLOCATION_FAILURE \
  "Allocation failure: Something went wrong! we couldn't allocate enough "\
  "memory for your program, pleas try again.\n"
// TODO: find were to insert this message in the code.

// COMPILATION & DECLARATION SECTION:

bool ok_arguments_amount (int argc);
int validate_input (int argc, char *argv[], int *seed, int *tweets_amount,
                    int *words_to_read);
bool parse_integer_from_string (int *changed_source, char *source);
int tweets_generator_logic (unsigned int seed, unsigned int tweets_number,
                            char *text_corpus_path, int words_to_read);
int fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain);

// _______________________________starts____________________________________ //

int main (int argc, char *argv[])
{
  // setting the params.
  int seed = TEMP_NUMBER;
  int tweets_amount = TEMP_NUMBER;
  int words_to_read = TEMP_NUMBER;
  char *text_corpus_path = NULL;
  if (validate_input (argc, argv, &seed, &tweets_amount, &words_to_read)
      == EXIT_FAILURE)
  { return EXIT_FAILURE; }

  text_corpus_path = argv[TEXT_CORPUS_PATH];

  return tweets_generator_logic (seed, tweets_amount,
                                 text_corpus_path, words_to_read);
}

int validate_input (int argc, char *argv[], int *seed, int *tweets_amount,
                    int *words_to_read)
{
  if (!ok_arguments_amount (argc))
  {
    fprintf (stdout, ERR_MSG_USAGE_PROBLEM);
    return EXIT_FAILURE;
  }

  if (parse_integer_from_string (seed, argv[SEED]) == false)
  {
    return EXIT_FAILURE;
  }
  if (parse_integer_from_string (tweets_amount, argv[TWEETS_NUMBER]) ==
      false)
  {
    return EXIT_FAILURE;
  }

  if (argc == FULL_AMOUNT_OF_ARGC)
  {
    if (parse_integer_from_string (words_to_read, argv[WORD_TO_READ]) ==
        false)
    {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

bool ok_arguments_amount (int argc)
{
  bool valid = true;
  if (!((argc == ACCEPTED_AMOUNT_OF_ARGC) || (argc == FULL_AMOUNT_OF_ARGC)))
  { valid = false; }

  return valid;
}

bool parse_integer_from_string (int *changed_source, char *source)
{
  bool flag = true;
  if (sscanf (source, "%d", changed_source) != 1)
  { flag = false; }

  return flag;
}

int tweets_generator_logic (unsigned int seed, unsigned int tweets_number,
                            char *text_corpus_path, int words_to_read)
{
// opening the file and define the seed.
  FILE *fp = fopen (text_corpus_path, "r");
  if (fp == NULL)
  {
    fprintf (stdout, ERR_MSG_FILE_PATH);
    return EXIT_FAILURE;
  }
  srand (seed);

  // defining the params.
  LinkedList linked_list = {NULL, NULL, 0};
  MarkovChain markov_chain = {&linked_list};
  MarkovChain *markov_chain_pointer = &markov_chain;

  int ans = fill_database (fp, words_to_read, markov_chain_pointer);
  if (ans == EXIT_SUCCESS)
  {
    for (unsigned int index_of_tweet = 0;
         index_of_tweet < tweets_number; index_of_tweet++)
    {
      fprintf (stdout, "Tweet %d: ", index_of_tweet + 1);
      generate_tweet (markov_chain_pointer, NULL, WORD_MAX_LENGTH);
    }

    fclose (fp);
    free_database (&markov_chain_pointer);
    return EXIT_SUCCESS;
  }

  fclose (fp);
  free_database (&markov_chain_pointer);
  return EXIT_FAILURE;
}

bool continue_reading (int count, int max)
{
  if (max == TEMP_NUMBER)
  {
    return true;
  }

  if (count < max)
  {
    return true;
  }
  return false;
}

/***
 * @param line the current line to parse and add to the chain
 * @param markov_chain the chain to add the words into
 * @return the amount of words added to the chain
 */
int parse_one_line (char *line, MarkovChain *markov_chain,
                    const int words_to_read, int word_count)
{
  char *current_word;
  Node *curr = NULL;
  Node *prev = NULL;
  current_word = strtok (line, DELIMITERS);
  while (current_word != NULL && continue_reading (word_count,
                                                   words_to_read))
  {
    curr = add_to_database (markov_chain, current_word);
    word_count++;

    if (prev)
    {
      add_node_to_frequencies_list ((prev)->data, (curr)->data);
    }

    prev = curr;
    current_word = strtok (NULL, DELIMITERS);
  }
  return word_count;
}

int fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  int word_count = 0;

  if (fp == NULL)
  {
    return EXIT_FAILURE;
  }

  char line_buffer[BUFFER_SIZE];

  while (fgets (line_buffer, BUFFER_SIZE, fp) != NULL)
  {
    word_count = parse_one_line (line_buffer, markov_chain,
                                 words_to_read, word_count);

    if (words_to_read != TEMP_NUMBER && word_count >= words_to_read)
    {
      break;
    }
  }

  return EXIT_SUCCESS;
}
