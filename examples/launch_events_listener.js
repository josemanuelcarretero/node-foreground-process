let focused_process_listener = require("../");

let launch_listener = ()=>{
  focused_process_listener.start((action, name) => {
    if(action === "focused_process"){
      console.log("focused_process", name, (new Date()).toISOString() )
    }
    else if(action === "interrupt" || action === "error"){
      focused_process_listener.stop();
      setTimeout(launch_listener, 500);
    }
  });
};
process.nextTick(launch_listener);
