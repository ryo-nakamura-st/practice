"use strict";

const fs = require("fs");
const ReadToken = require("readtoken");
const StringData = require("./stringData");

class TokenComparator {
  static VALID = -1;
  static INVALID_DATA_LENGTH = -2;
  constructor(eps) {
    this.eps = eps || -1;
  }

  compareTokens(token1, token2) {
    if (token1 === token2) {
      return true;
    }

    if (this.eps > 0) {
      const n1 = Number(token1);
      const n2 = Number(token2);
      return Math.abs(n1 - n2) <= this.eps;
    }

    return false;
  }

  compareStrings(str1, str2) {
    const tokens1 = StringData.fromRaw(str1).lines().flatMap(l => l.split(',').map(t => t.trim()));
    const tokens2 = StringData.fromRaw(str2).lines().flatMap(l => l.split(',').map(t => t.trim()));
    const len = Math.min(tokens1.length, tokens2.length);
    if (tokens1.length !== tokens2.length) {
      return {
        index: len,
        token1: tokens1[len - 1],
        token2: tokens2[len - 1]
      };
    }
    let index = 0;
    while (index < len) {
      if (!this.compareTokens(tokens1[index], tokens2[index])) {
        return {
          index: index + 1,
          token1: tokens1[index],
          token2: tokens2[index]
        };
      }
      index++;
    }
    return {
      index: TokenComparator.VALID
    };
  }
  compareFiles(filepath1, filepath2) {
    const self = this;
    const tokens1 = [];
    const tokens2 = [];
    let index = 0;
    let file1Closed = false;
    let file2Closed = false;
    let file1Read = false;
    let file2Read = false;
    return new Promise((resolve, reject) => {
      try {
        function fireCompare() {
          if (tokens1.length > 0 && tokens2.length > 0) {
            const len = Math.min(tokens1.length, tokens2.length);
            for (let i=0; i<len; i++) {
              index++;
              const a = tokens1[i];
              const b = tokens2[i];
              if (!self.compareTokens(a, b)) {
                forceClose(index, a, b);
                return false;
              }
            }
            tokens1.splice(0, len);
            tokens2.splice(0, len);
          }
          return true;
        }
        function forceClose(index, a, b) {
          if (!file1Closed) {
            file1Closed = true;
            rl1.close();
          }
          if (!file2Closed) {
            file2Closed = true;
            rl2.close();
          }
          resolve({
            index, 
            token1: a,
            token2: b
          });
        }
        function doClose() {
          if (file1Closed && file2Closed && fireCompare()) {
            if (tokens1.length === 0 && tokens2.length === 0) {
              resolve({
                index: TokenComparator.VALID
              });
            } else {
              resolve({
                index: index + 1,
                token1: tokens1[0],
                token2: tokens2[0],
                file1Unread: !file1Read,
                file2Unread: !file2Read
              });
            }
          }
        }
        const rl1 = new ReadToken(fs.createReadStream(filepath1, { encoding: "utf-8" }), { maxLength: 100, readSize: 8192 });
        const rl2 = new ReadToken(fs.createReadStream(filepath2, { encoding: "utf-8" }), { maxLength: 100, readSize: 8192 });
        rl1.on('token', token => {
          file1Read = true;
          token.split(',').forEach(t => tokens1.push(t.trim()));
          fireCompare();
        });
        rl2.on('token', token => {
          file2Read = true;
          token.split(',').forEach(t => tokens2.push(t.trim()));
          fireCompare();
        });
        rl1.on("close", () => {
          if (file1Closed) {
            return;
          }
          file1Closed = true;
          doClose();
        });
        rl2.on("close", () => {
          if (file2Closed) {
            return;
          }
          file2Closed = true;
          doClose();
        });
      } catch (e) {
        reject(e);
      }
    });
  }
}

module.exports = TokenComparator;
