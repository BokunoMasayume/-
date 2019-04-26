const {app,BrowserWindow,Menu} = require('electron');


function createWindow(){
	Menu.setApplicationMenu(null);

	win = new BrowserWindow({width:400,
							 height:270});

	win.loadFile('hello.html');

	//win.webContents.openDevTools();
	win.on('close',()=>{
		win = null;
	});


}

app.on('ready' , createWindow);
