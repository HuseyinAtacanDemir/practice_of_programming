const NPREF = 2, MAXGEN = 100, NONWORD = '\n', map = new Map();
let input = '';

process.stdin.on('data', d => input += d).on('end', () => {
    let prefix = Array(NPREF).fill(NONWORD).join(' ');
    input
      .split(/\s+/)
      .filter(Boolean)
      .forEach(word => (map.set(prefix, [...(map.get(prefix) || []), word]), prefix = prefix.split(' ').slice(1).concat(word).join(' ')));
    
    prefix = Array(NPREF).fill(NONWORD).join(' ');
    for (let i = 0; i < MAXGEN && map.get(prefix); i++) {
        let word = map.get(prefix)[Math.floor(Math.random() * map.get(prefix).length)];
        if (word === NONWORD) break;
        process.stdout.write(word + ' ');
        prefix = prefix
                  .split(' ')
                  .slice(1)
                  .concat(word)
                  .join(' ');
    }
});

