const http = require('http');

let rank = [];

const server = http.createServer((req, res) => {
  console.log(req.method);
  if (req.method === 'POST') {
    let buffer = '';
    req.on('data', chunk => {
      buffer += chunk;
    });
    req.on('end', () => {
      console.log(buffer);
      addRank(JSON.parse(buffer));
    });
    res.end(JSON.stringify({ status: true }));
  } else {
    const result = JSON.stringify(getRank());
    console.log(result);
    res.end(result);
  }

  function getRank() {
    return { status: true, rank };
  }

  function addRank(to) {
    const index = rank.findIndex(value => value.name === to.name);
    if (index !== -1) rank[index].score = to.score;
    else rank.push(to);
    rank = rank
      .sort((a, b) => {
        return a.score < b.score;
      })
      .slice(0, 9);
  }
});

server.listen(8080);

console.log('Listen at 127.0.0.1:8080');
