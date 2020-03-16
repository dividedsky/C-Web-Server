#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "cache.h"

/**
 * Allocate a cache entry
 */
struct cache_entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
  struct cache_entry *ce = malloc(sizeof(struct cache_entry));
  ce->path = strdup(path);
  ce->content_type = strdup(content_type);
  ce->content_length = content_length;
  ce->content = content;
  /* sean did the below in lecture, but it broke my tests
  ce->content = malloc(content_length);
  memcpy(ce->content, content, content_length);
  */
}

/**
 * Deallocate a cache entry
 */
void free_entry(struct cache_entry *entry)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
  free(entry->path);
  free(entry->content_type);
  free(entry);
}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(struct cache *cache, struct cache_entry *ce)
{
    // Insert at the head of the list
    if (cache->head == NULL) {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    } else {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(struct cache *cache, struct cache_entry *ce)
{
    if (ce != cache->head) {
        if (ce == cache->tail) {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;

        } else {
            // We're neither the head nor the tail
            ce->prev->next = ce->next;
            ce->next->prev = ce->prev;
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}


/**
 * Removes the tail from the list and returns it
 * 
 * NOTE: does not deallocate the tail
 */
struct cache_entry *dllist_remove_tail(struct cache *cache)
{
    struct cache_entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 * 
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
struct cache *cache_create(int max_size, int hashsize)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
  struct cache *cache = malloc(sizeof(*cache));
  cache->index = hashtable_create(hashsize, NULL); // pass NULL to use default hash function
  cache->head = NULL;
  cache->tail = NULL;
  cache->max_size = max_size;
  cache->cur_size = 0;
}

void cache_free(struct cache *cache)
{
    struct cache_entry *cur_entry = cache->head;

    hashtable_destroy(cache->index);

    while (cur_entry != NULL) {
        struct cache_entry *next_entry = cur_entry->next;

        free_entry(cur_entry);

        cur_entry = next_entry;
    }

    free(cache);
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 * 
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(struct cache *cache, char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
  // allocate new cache entry
  struct cache_entry *ce = alloc_entry(path, content_type, content, content_length);

  // insert entry at head of LL
  dllist_insert_head(cache, ce);

  // store entry in hash table, indexed by path
  hashtable_put(cache->index, path, ce);

  // increment cache size
  cache->cur_size++;

  // check if size is greater than max size
  if (cache->cur_size > cache->max_size) {
    // pointer copy to the tail
    struct cache_entry *oldtail = dllist_remove_tail(cache);
    // remove the tail
    // remove from hash table
    hashtable_delete(cache->index, oldtail->path);
    // free old tail
    free_entry(oldtail);
    // update cache size--NOT necessary because free_entry does this
    /* cache->cur_size--; */
  }
}

/**
 * Retrieve an entry from the cache
 */
struct cache_entry *cache_get(struct cache *cache, char *path)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
  // look up path in hash table
  struct cache_entry *ce = hashtable_get(cache->index, path);

  // return null if not found
  if (ce == NULL) {
    return NULL;
  }

  // move entry to head
  dllist_move_to_head(cache, ce);

  // return entry pointer
  return ce;
}
