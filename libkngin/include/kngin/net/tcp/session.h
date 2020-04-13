/*
 * static
 * void
 * session::broadcast (session_list &list, out_buffer buf) {
 *   assert(list.size());
 *   assert(buf.size());
 *
 *   for (auto &iter : list) {
 *     if (iter.established())
 *       iter.async_write(buf);
 *   }
 * }
 * */