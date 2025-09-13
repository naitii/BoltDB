async function post(path, data){
  const body = new URLSearchParams(data).toString();
  const res = await fetch(path,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body});
  return res.json();
}

async function getJSON(url){
  const res = await fetch(url);
  return res.json();
}

document.getElementById('set-btn').addEventListener('click', async ()=>{
  const key = document.getElementById('set-key').value;
  const value = document.getElementById('set-value').value;
  const out = document.getElementById('set-result');
  out.textContent = 'Saving...';
  try{
    const r = await post('/api/set',{key,value});
    out.textContent = JSON.stringify(r,null,2);
  }catch(e){out.textContent = 'Error: '+e}
});

document.getElementById('get-btn').addEventListener('click', async ()=>{
  const key = document.getElementById('get-key').value;
  const out = document.getElementById('get-result');
  out.textContent = 'Fetching...';
  try{
    const r = await getJSON('/api/get?key='+encodeURIComponent(key));
    out.textContent = JSON.stringify(r,null,2);
  }catch(e){out.textContent = 'Not found'}
});

document.getElementById('del-btn').addEventListener('click', async ()=>{
  const key = document.getElementById('del-key').value;
  const out = document.getElementById('del-result');
  out.textContent = 'Deleting...';
  try{
    const r = await post('/api/delete',{key});
    out.textContent = JSON.stringify(r,null,2);
  }catch(e){out.textContent = 'Error: '+e}
});


