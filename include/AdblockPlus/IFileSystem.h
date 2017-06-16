/*
 * This file is part of Adblock Plus <https://adblockplus.org/>,
 * Copyright (C) 2006-2017 eyeo GmbH
 *
 * Adblock Plus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * Adblock Plus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Adblock Plus.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ADBLOCK_PLUS_IFILE_SYSTEM_H
#define ADBLOCK_PLUS_IFILE_SYSTEM_H

#include <istream>
#include <stdint.h>
#include <string>
#include <memory>
#include <vector>

namespace AdblockPlus
{
  /**
   * File system interface.
   */
  class IFileSystem
  {
  public:
    /**
     * Result of a stat operation, i.e.\ information about a file.
     */
    struct StatResult
    {
      StatResult()
      {
        exists = false;
        isDirectory = false;
        isFile = false;
        lastModified = 0;
      }

      /**
       * File exists.
       */
      bool exists;

      /**
       * File is a directory.
       */
      bool isDirectory;

      /**
       * File is a regular file.
       */
      bool isFile;

      /**
       * POSIX time of the last modification.
       */
      int64_t lastModified;
    };

    virtual ~IFileSystem() {}

    /** Type for the buffer used for IO */
    typedef std::vector<uint8_t> IOBuffer;

    /**
     * Default callback type for asynchronous filesystem calls.
     * @param An error string. Empty is success.
     */
    typedef std::function<void(const std::string&)> Callback;

    /**
     * Callback type for the asynchronous Read call.
     * @param Output char array with file content.
     * @param An error string. Empty if success.
     */
    typedef std::function<void(IOBuffer&&,
                               const std::string&)> ReadCallback;

    /**
     * Reads from a file.
     * @param path File path.
     * @param callback The function called on completion with the input data.
     */
    virtual void Read(const std::string& path,
                      const ReadCallback& callback) const = 0;

    /**
     * Writes to a file.
     * @param path File path.
     * @param data The data to write.
     * @param callback The function called on completion.
     */
    virtual void Write(const std::string& path,
                       const IOBuffer& data,
                       const Callback& callback) = 0;

    /**
     * Moves a file (i.e.\ renames it).
     * @param fromPath Current path to the file.
     * @param toPath New path to the file.
     * @param callback The function called on completion.
     */
    virtual void Move(const std::string& fromPath, const std::string& toPath,
                      const Callback& callback) = 0;

    /**
     * Removes a file.
     * @param path File path.
     * @param callback The function called on completion.
     */
    virtual void Remove(const std::string& path, const Callback& callback) = 0;

    /**
     * Callback type for the asynchronous Stat call.
     * @param the StatResult data.
     * @param an error string. Empty if no error.
     */
    typedef std::function<void(const StatResult&, const std::string&)> StatCallback;

    /**
     * Retrieves information about a file.
     * @param path File path.
     * @param callback The function called on completion.
     */
    virtual void Stat(const std::string& path,
                      const StatCallback& callback) const = 0;

    /**
     * Returns the absolute path to a file.
     * @param path File path (can be relative or absolute).
     * @return Absolute file path.
     */
    virtual std::string Resolve(const std::string& path) const = 0;
  };

  /**
   * Shared smart pointer to a `IFileSystem` instance.
   */
  typedef std::shared_ptr<IFileSystem> FileSystemPtr;
}

#endif